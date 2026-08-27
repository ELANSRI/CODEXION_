/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamaarab <hamaarab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 03:36:19 by hamaarab          #+#    #+#             */
/*   Updated: 2026/04/19 06:46:08 by hamaarab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	create_threads(t_data *data, t_coder *coders, t_dongle *dongles)
{
	int	i;
	int	j;

	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL, &coder_routine,
				&coders[i]) != 0)
		{
			safe_stop(data, 1);
			j = 0;
			while (j < i)
			{
				pthread_join(coders[j].thread_id, NULL);
				j++;
			}
			cleanup_simulation(data, coders, dongles);
			return (0);
		}
		i++;
	}
	return (1);
}

int	join_threads(t_data *data, t_coder *coders, t_dongle *dongles)
{
	int	i;
	int	safe;

	safe = 1;
	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_join(coders[i].thread_id, NULL) != 0)
			safe = 0;
		i++;
	}
	cleanup_simulation(data, coders, dongles);
	return (safe == 1);
}
