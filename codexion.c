#include "codexion.h"

void	cleanup_simulation(t_data *data, t_coder *coders, t_dongle *dongles)
{
	int	i;

	if (dongles)
	{
		i = 0;
		while (i < data->number_of_coders)
		{
			pthread_mutex_destroy(&dongles[i].dongle_mutex);
			pthread_cond_destroy(&dongles[i].cond); // NOUVEAU
			if (dongles[i].queue)
				free(dongles[i].queue);
			i++;
		}
		free(dongles);
	}
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->stop_mutex);
	pthread_mutex_destroy(&data->sched_mutex);
	if (coders)
		free(coders);
}

int	main(int ac, char **av)
{
	t_data		data;
	t_coder		*coders;
	t_dongle	*dongles;
	pthread_t	monitor_id;

	if (!parse_args(ac, av, &data))
		return (1);
	if (!init_simulation(&data, &coders, &dongles))
		return (1);
	if (!create_threads(&data, coders, dongles))
		return (1);
	if (pthread_create(&monitor_id, NULL, monitor_routine, coders) != 0)
	{
		safe_stop(&data, 1);
		join_threads(&data, coders, dongles);
		return (1);
	}
	pthread_join(monitor_id, NULL);
	if (!join_threads(&data, coders, dongles))
		return (1);
	return (0);
}