#include "codexion.h"

int	error(char *error_msg)
{
	fprintf(stderr, "Error: %s\n", error_msg);
	return (0);
}

void	safe_increment(t_coder *coder)
{
	pthread_mutex_lock(&coder->data->sched_mutex);
	coder->compile_done++;
	pthread_mutex_unlock(&coder->data->sched_mutex);
}

void	safe_print(t_data *data, int id, char *msg)
{
	pthread_mutex_lock(&data->print_mutex);
	pthread_mutex_lock(&data->stop_mutex);
	if (!data->stop)
	{
		printf("%ld %d %s\n", get_time() - data->start_time, id, msg);
	}
	pthread_mutex_unlock(&data->stop_mutex);
	pthread_mutex_unlock(&data->print_mutex);
}

void	safe_stop(t_data *data, int value)
{
	pthread_mutex_lock(&data->stop_mutex);
	data->stop = value;
	pthread_mutex_unlock(&data->stop_mutex);
}

int	check_stop(t_data *data)
{
	int	stop;

	pthread_mutex_lock(&data->stop_mutex);
	stop = data->stop;
	pthread_mutex_unlock(&data->stop_mutex);
	return (stop);
}

// NOUVEAU : Remplace usleep() pour une précision parfaite
void	precise_sleep(t_data *data, long time_in_ms)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms)
	{
		if (check_stop(data))
			break ;
		usleep(250);
	}
}