/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 21:23:19 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/11 21:31:11 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char **argv)
{
	t_config config;

	if (argc != 9)
	{
		printf("ERROR: invalid number of arguments\n");
		return (1);
	}
	config = parse_args(argv);
	printf("Configuration:\n");
	printf("Number of coders: %d\n", config.number_of_coders);
	printf("Time to burnout: %ld\n", config.time_to_burnout);
	printf("Time to compile: %ld\n", config.time_to_compile);
	printf("Time to debug: %ld\n", config.time_to_debug);
	printf("Time to refactor: %ld\n", config.time_to_refactor);
	printf("Number of compiles required: %d\n",
		config.number_of_compiles_required);
	printf("Dongle cooldown: %ld\n", config.dongle_cooldown);
	return (0);
}