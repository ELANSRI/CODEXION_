#include "codexion.h"

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	int	i;

	pthread_mutex_lock(&dongle->dongle_mutex);
	dongle->last_used = get_time();
	if (strcmp(coder->data->scheduler, "edf") == 0)
		heap_pop(dongle);
	else
	{
		i = 0;
		while (i < dongle->size - 1)
		{
			dongle->queue[i] = dongle->queue[i + 1];
			i++;
		}
		dongle->size--;
	}
	pthread_cond_broadcast(&dongle->cond); // NOUVEAU : Réveille les suivants
	pthread_mutex_unlock(&dongle->dongle_mutex);
}

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_mutex);
	
	// Figer la priorité au moment de la demande
	pthread_mutex_lock(&coder->data->sched_mutex);
	if (strcmp(coder->data->scheduler, "edf") == 0)
		coder->current_priority = coder->last_compile + coder->data->time_to_burnout;
	else
		coder->current_priority = get_time(); // FIFO : Heure d'arrivée
	pthread_mutex_unlock(&coder->data->sched_mutex);

	if (strcmp(coder->data->scheduler, "edf") == 0)
		heap_push(dongle, coder);
	else
		dongle->queue[dongle->size++] = coder;

	while (!check_stop(coder->data))
	{
		if (scheduler(coder, dongle))
		{
			safe_print(coder->data, coder->id, "has taken a dongle");
			pthread_mutex_unlock(&dongle->dongle_mutex);
			return ;
		}
		// NOUVEAU : On dort proprement en relâchant le mutex
		pthread_cond_wait(&dongle->cond, &dongle->dongle_mutex);
	}
	pthread_mutex_unlock(&dongle->dongle_mutex);
}

void	take_dongles(t_coder *coder)
{
	// NOUVEAU : Gestion parfaite du cas 1 codeur
	if (coder->data->number_of_coders == 1)
	{
		take_dongle(coder, coder->left_dongle);
		while (!check_stop(coder->data))
			usleep(1000);
		return ;
	}
	if (coder->id % 2 == 0)
	{
		take_dongle(coder, coder->right_dongle);
		take_dongle(coder, coder->left_dongle);
	}
	else
	{
		take_dongle(coder, coder->left_dongle);
		take_dongle(coder, coder->right_dongle);
	}
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!check_stop(coder->data))
	{
		take_dongles(coder);
		if (check_stop(coder->data))
			break ;

		pthread_mutex_lock(&coder->data->sched_mutex);
		coder->last_compile = get_time();
		pthread_mutex_unlock(&coder->data->sched_mutex);
		
		safe_print(coder->data, coder->id, "is compiling");
		precise_sleep(coder->data, coder->data->time_to_compile);
		
		release_dongle(coder, coder->left_dongle);
		if (coder->data->number_of_coders > 1)
			release_dongle(coder, coder->right_dongle);
			
		safe_increment(coder);
		
		safe_print(coder->data, coder->id, "is debugging");
		precise_sleep(coder->data, coder->data->time_to_debug);
		
		safe_print(coder->data, coder->id, "is refactoring");
		precise_sleep(coder->data, coder->data->time_to_refactor);
	}
	return (NULL);
}