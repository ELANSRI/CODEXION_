#include "codexion.h"

void	cleanup_simulation(t_data *data, t_coder *coders, t_dongle *dongles)
{
	if (dongles)
		free(dongles);
	
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->stop_mutex);
	pthread_mutex_destroy(&data->sched_mutex);
	pthread_mutex_destroy(&data->table_mutex);
	pthread_cond_destroy(&data->table_cond);
	
	if (coders)
		free(coders);
}

int	create_threads(t_data *data, t_coder *coders, t_dongle *dongles)
{
	int	i = 0;

	while (i < data->number_of_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL, &coder_routine, &coders[i]) != 0)
		{
			safe_stop(data, 1);
			while (--i >= 0)
				pthread_join(coders[i].thread_id, NULL);
			cleanup_simulation(data, coders, dongles);
			return (0);
		}
		i++;
	}
	return (1);
}

int	join_threads(t_data *data, t_coder *coders, t_dongle *dongles)
{
	int	i = 0;
	int	safe = 1;

	while (i < data->number_of_coders)
	{
		if (pthread_join(coders[i].thread_id, NULL) != 0)
			safe = 0;
		i++;
	}
	cleanup_simulation(data, coders, dongles);
	return (safe == 1);
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