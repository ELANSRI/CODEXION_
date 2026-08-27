#include "codexion.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	init_dongles(t_data *data, t_dongle *dongles)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		dongles[i].id = i;
		dongles[i].last_used = 0;
		if (pthread_mutex_init(&dongles[i].dongle_mutex, NULL) != 0)
			return (0);
		if (pthread_cond_init(&dongles[i].cond, NULL) != 0)
			return (0); // NOUVEAU
		dongles[i].capacity = data->number_of_coders; // Sécurité max
		dongles[i].size = 0;
		dongles[i].queue = malloc(sizeof(t_coder *) * dongles[i].capacity);
		if (!dongles[i].queue)
			return (0);
		i++;
	}
	return (1);
}

void	init_coders(t_data *data, t_coder *coders, t_dongle *dongles)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].compile_done = 0;
		coders[i].last_compile = data->start_time;
		coders[i].current_priority = 0;
		coders[i].data = data;
		coders[i].left_dongle = &dongles[i];
		coders[i].right_dongle = &dongles[(i + 1) % data->number_of_coders];
		i++;
	}
}

int	init_simulation(t_data *data, t_coder **coders, t_dongle **dongles)
{
	*coders = malloc(sizeof(t_coder) * data->number_of_coders);
	*dongles = malloc(sizeof(t_dongle) * data->number_of_coders);
	if (!*coders || !*dongles)
		return (0);
	data->start_time = get_time();
	data->stop = 0;
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->stop_mutex, NULL);
	pthread_mutex_init(&data->sched_mutex, NULL);
	if (!init_dongles(data, *dongles))
	{
		cleanup_simulation(data, *coders, *dongles);
		return (0);
	}
	init_coders(data, *coders, *dongles);
	return (1);
}