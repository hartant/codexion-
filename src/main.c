/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 21:23:19 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/13 22:10:44 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"


int	create_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
			return (0);
		i++;
	}
	return (1);
}

void	join_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	*sim;
	pthread_t		monitor_thread;

	if (argc != 9)
	{
		printf("ERROR: invalid number of arguments\n");
		return (1);
	}
	config = parse_args(argv);
	sim = init_simulation(config);
	if (!sim)
	{
		printf("ERROR: initialization failed\n");
		return (1);
	}
	if (pthread_create(&monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		printf("ERROR: monitor thread creation failed\n");
		cleanup_simulation(sim);
		return (1);
	}
	if (!create_threads(sim))
	{
		printf("ERROR: thread creation failed\n");
		cleanup_simulation(sim);
		return (1);
	}
	join_threads(sim);
	pthread_join(monitor_thread, NULL);
	cleanup_simulation(sim);
	return (0);
}