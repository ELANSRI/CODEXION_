/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamaarab <hamaarab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 03:36:37 by hamaarab          #+#    #+#             */
/*   Updated: 2026/04/19 07:04:14 by hamaarab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_scheduler(char *str, t_data *data)
{
	if (strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0)
		data->scheduler = str;
	else
		return (0);
	return (1);
}

int	ft_is_number(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	i = 0;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

char	*error_table(int i)
{
	char	*table[8];

	table[0] = "invalid number_of_coders";
	table[1] = "invalid time_to_burnout";
	table[2] = "invalid time_to_compile";
	table[3] = "invalid time_to_debug";
	table[4] = "invalid time_to_refactor";
	table[5] = "invalid number_of_compiles_required";
	table[6] = "invalid dongle_cooldown";
	table[7] = "invalid scheduler";
	return (table[i]);
}

int	init_all(char **av, t_data *data)
{
	data->number_of_coders = atoi(av[1]);
	data->time_to_burnout = atoi(av[2]);
	data->time_to_compile = atoi(av[3]);
	data->time_to_debug = atoi(av[4]);
	data->time_to_refactor = atoi(av[5]);
	data->required_compiles = atoi(av[6]);
	data->dongle_cooldown = atoi(av[7]);
	if (data->number_of_coders <= 0)
		return (error(error_table(0)));
	if (data->time_to_burnout <= 0)
		return (error(error_table(1)));
	if (data->time_to_compile <= 0)
		return (error(error_table(2)));
	if (data->time_to_debug <= 0)
		return (error(error_table(3)));
	if (data->time_to_refactor <= 0)
		return (error(error_table(4)));
	if (data->required_compiles <= 0)
		return (error(error_table(5)));
	if (data->dongle_cooldown < 0)
		return (error(error_table(6)));
	return (1);
}

int	parse_args(int ac, char **av, t_data *data)
{
	int	i;

	if (ac != 9)
		return (error("Wrong number of arguments!\n"));
	i = 1;
	while (i < 8)
	{
		if (!ft_is_number(av[i]))
			return (error(error_table(i - 1)));
		i++;
	}
	if (!parse_scheduler(av[i], data))
		return (error(error_table(7)));
	if (!init_all(av, data))
		return (0);
	return (1);
}
