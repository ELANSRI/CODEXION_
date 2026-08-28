#include "codexion.h"

// Vérifie si un codeur PEUT matériellement prendre ses 2 dongles
static int can_take(t_coder *c, long now)
{
	if (c->left_dongle->in_use || now < c->left_dongle->available_at)
		return (0);
	if (c->right_dongle->in_use || now < c->right_dongle->available_at)
		return (0);
	return (1);
}

// Vérifie si quelqu'un de plus prioritaire ET capable de compiler est en attente
static int is_highest_priority(t_coder *c, long now)
{
	int		i = 0;
	t_coder	*other;

	while (i < c->data->number_of_coders)
	{
		other = &c->data->all_coders[i];
		// Si un AUTRE codeur est en train d'ATTENDRE
		if (other->id != c->id && other->state == 1)
		{
			// S'ils partagent un dongle (Conflit)
			if (other->left_dongle == c->left_dongle || other->left_dongle == c->right_dongle ||
				other->right_dongle == c->left_dongle || other->right_dongle == c->right_dongle)
			{
				int other_higher = 0;
				if (other->current_priority < c->current_priority)
					other_higher = 1;
				else if (other->current_priority == c->current_priority && other->id < c->id)
					other_higher = 1;

				// Si l'autre est plus prioritaire ET que ses 2 dongles sont libres, on s'écrase
				if (other_higher && can_take(other, now))
					return (0);
			}
		}
		i++;
	}
	return (1); // Personne de plus urgent ne nous bloque
}

void	take_dongles(t_coder *coder)
{
	long	now;

	if (coder->data->number_of_coders == 1)
	{
		safe_print(coder->data, coder->id, "has taken a dongle");
		while (!check_stop(coder->data))
			precise_sleep(coder->data, 10);
		return ;
	}

	pthread_mutex_lock(&coder->data->sched_mutex);
	if (strcmp(coder->data->scheduler, "edf") == 0)
		coder->current_priority = coder->last_compile + coder->data->time_to_burnout;
	else
		coder->current_priority = get_time();
	pthread_mutex_unlock(&coder->data->sched_mutex);

	pthread_mutex_lock(&coder->data->table_mutex);
	coder->state = 1; // Le codeur annonce qu'il attend

	while (!check_stop(coder->data))
	{
		now = get_time();
		if (can_take(coder, now))
		{
			if (is_highest_priority(coder, now))
			{
				coder->left_dongle->in_use = 1;
				coder->right_dongle->in_use = 1;
				coder->state = 0; // Il n'attend plus, il mange
				
				pthread_mutex_lock(&coder->data->print_mutex);
				pthread_mutex_lock(&coder->data->stop_mutex);
				if (!coder->data->stop)
				{
					long t = get_time() - coder->data->start_time;
					printf("%ld %d has taken a dongle\n", t, coder->id);
					printf("%ld %d has taken a dongle\n", t, coder->id);
					printf("%ld %d is compiling\n", t, coder->id);
				}
				pthread_mutex_unlock(&coder->data->stop_mutex);
				pthread_mutex_unlock(&coder->data->print_mutex);
				break ;
			}
		}
		pthread_cond_wait(&coder->data->table_cond, &coder->data->table_mutex);
	}
	
	if (check_stop(coder->data))
		coder->state = 0;
	pthread_mutex_unlock(&coder->data->table_mutex);
}

void	release_dongles(t_coder *coder)
{
	long	cooldown_end;

	cooldown_end = get_time() + coder->data->dongle_cooldown;
	
	pthread_mutex_lock(&coder->data->table_mutex);
	coder->left_dongle->in_use = 0;
	coder->left_dongle->available_at = cooldown_end;
	coder->right_dongle->in_use = 0;
	coder->right_dongle->available_at = cooldown_end;
	
	pthread_cond_broadcast(&coder->data->table_cond);
	pthread_mutex_unlock(&coder->data->table_mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder = (t_coder *)arg;

	while (!check_stop(coder->data))
	{
		take_dongles(coder);
		if (check_stop(coder->data))
			break ;
		
		pthread_mutex_lock(&coder->data->sched_mutex);
		coder->last_compile = get_time();
		pthread_mutex_unlock(&coder->data->sched_mutex);
		
		precise_sleep(coder->data, coder->data->time_to_compile);
		
		release_dongles(coder);
		safe_increment(coder);
		
		safe_print(coder->data, coder->id, "is debugging");
		precise_sleep(coder->data, coder->data->time_to_debug);
		
		safe_print(coder->data, coder->id, "is refactoring");
		precise_sleep(coder->data, coder->data->time_to_refactor);
	}
	return (NULL);
}