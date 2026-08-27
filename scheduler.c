/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamaarab <hamaarab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 03:36:43 by hamaarab          #+#    #+#             */
/*   Updated: 2026/04/19 23:15:16 by hamaarab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_deadline(t_coder *coder)
{
	long	deadline;

	pthread_mutex_lock(&coder->data->sched_mutex);
	deadline = coder->last_compile + coder->data->time_to_burnout;
	pthread_mutex_unlock(&coder->data->sched_mutex);
	return (deadline);
}

int	scheduler(t_coder *coder, t_dongle *dongle)
{
	long	now;

	if (dongle->size == 0)
		return (0);
	if (dongle->queue[0] != coder)
		return (0);
	now = get_time();
	if (now - dongle->last_used < coder->data->dongle_cooldown)
		return (0);
	return (1);
}
