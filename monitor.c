#include "codexion.h"

int	is_coder_burned_out(t_data *data, t_coder *coder, long now)
{
	if (now - coder->last_compile > data->time_to_burnout + 5)
	{
		pthread_mutex_lock(&data->print_mutex);
		pthread_mutex_lock(&data->stop_mutex);
		if (!data->stop)
		{
			data->stop = 1;
			printf("%ld %d burned out\n", now - data->start_time, coder->id);
		}
		pthread_mutex_unlock(&data->stop_mutex);
		pthread_mutex_unlock(&data->print_mutex);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders = (t_coder *)arg;
	t_data	*data = coders[0].data;
	int		i;
	long	now;

	while (!check_stop(data))
	{
		now = get_time();
		i = 0;
		while (i < data->number_of_coders)
		{
			pthread_mutex_lock(&data->sched_mutex);
			if (coders[i].compile_done < data->required_compiles && is_coder_burned_out(data, &coders[i], now))
			{
				pthread_mutex_unlock(&data->sched_mutex);
				pthread_mutex_lock(&data->table_mutex);
				pthread_cond_broadcast(&data->table_cond);
				pthread_mutex_unlock(&data->table_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(&data->sched_mutex);
			i++;
		}
		
		pthread_mutex_lock(&data->table_mutex);
		pthread_cond_broadcast(&data->table_cond);
		pthread_mutex_unlock(&data->table_mutex);
		
		usleep(1000); 
	}
	return (NULL);
}