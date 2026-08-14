/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 22:23:20 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/14 23:11:28 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	init_dongle(t_dongle *dongle, int id, int n)
{
	dongle->id = id;
	pthread_mutex_init(&dongle->lock, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	dongle->available = 1;
	dongle->last_release_time = 0;
	dongle->waiting = malloc(sizeof(t_request) * n);
	dongle->waiting_count = 0;
}

static void	init_coder(t_simulation *sim, int i)
{
	int	n;

	n = sim->config.number_of_coders;
	sim->coders[i].id = i + 1;
	sim->coders[i].left_dongle = &sim->dongles[i];
	sim->coders[i].right_dongle = &sim->dongles[(i + 1) % n];
	sim->coders[i].last_compile_start = sim->start_time;
	sim->coders[i].compiles_done = 0;
	sim->coders[i].sim = sim;
}

static t_simulation	*alloc_simulation(t_config config)
{
	t_simulation	*sim;

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
	return (sim);
}

t_simulation	*init_simulation(t_config config)
{
	t_simulation	*sim;
	int				i;

	sim = alloc_simulation(config);
	if (!sim)
		return (NULL);
	pthread_mutex_init(&sim->print_lock, NULL);
	pthread_mutex_init(&sim->stop_lock, NULL);
	sim->stop = 0;
	sim->start_time = get_current_time();
	i = 0;
	while (i < config.number_of_coders)
	{
		init_dongle(&sim->dongles[i], i, config.number_of_coders);
		init_coder(sim, i);
		i++;
	}
	return (sim);
}

int	interruptible_sleep(t_simulation *sim, long ms)
{
	long	remaining;
	long	chunk;
	int		stop;

	remaining = ms;
	while (remaining > 0)
	{
		if (remaining < 2)
			chunk = remaining;
		else
			chunk = 2;
		usleep(chunk * 1000);
		remaining -= chunk;
		pthread_mutex_lock(&sim->stop_lock);
		stop = sim->stop;
		pthread_mutex_unlock(&sim->stop_lock);
		if (stop)
			return (0);
	}
	return (1);
}
