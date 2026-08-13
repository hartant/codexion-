/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 22:23:20 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/13 22:11:40 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

t_simulation	*init_simulation(t_config config)
{
	t_simulation *sim;
	int i;

	sim = malloc(sizeof(t_simulation));
	if (!sim)
		return (NULL);
	sim->config = config;
	sim->coders = malloc(sizeof(t_coder) * config.number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * config.number_of_coders);
	if (!sim->coders || !sim->dongles)
	{
		free(sim->coders);
		free(sim->dongles);
		free(sim);
		return (NULL);
	}
	pthread_mutex_init(&sim->print_lock, NULL);
	pthread_mutex_init(&sim->stop_lock, NULL);
	sim->stop = 0;
	sim->start_time = get_current_time();
	i = 0;
	while (i < config.number_of_coders)
	{
		sim->dongles[i].id = i;
		pthread_mutex_init(&sim->dongles[i].lock, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].available = 1;
		sim->dongles[i].last_release_time = 0;
		sim->dongles[i].waiting = malloc(sizeof(t_request) * config.number_of_coders);
		sim->dongles[i].waiting_count = 0;
		sim->coders[i].id = i;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1)
			% config.number_of_coders];
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].sim = sim;
		i++;
	}
	return (sim);
}