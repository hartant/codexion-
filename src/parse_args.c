/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 21:21:35 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/12 21:12:34 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

t_scheduler	parse_scheduler(char *str)
{
	if (strcmp(str, "fifo") == 0)
		return (fifo);
	else if (strcmp(str, "edf") == 0)
		return (edf);
	printf("ERROR: scheduler must be fifo or edf\n");
	exit(1);
}

void	fill_config(t_config *config, char **argv)
{
	config->number_of_coders = (int)ft_atoll(argv[1]);
	config->time_to_burnout = ft_atoll(argv[2]);
	config->time_to_compile = ft_atoll(argv[3]);
	config->time_to_debug = ft_atoll(argv[4]);
	config->time_to_refactor = ft_atoll(argv[5]);
	config->number_of_compiles_required = (int)ft_atoll(argv[6]);
	config->dongle_cooldown = ft_atoll(argv[7]);
	config->scheduler = parse_scheduler(argv[8]);
}

t_config	parse_args(char **argv)
{
	t_config	config;

	check_positive(argv[1]);
	check_digits(argv[2]);
	check_digits(argv[3]);
	check_digits(argv[4]);
	check_digits(argv[5]);
	check_positive(argv[6]);
	check_digits(argv[7]);
	fill_config(&config, argv);
	return (config);
}
