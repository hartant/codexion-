/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 18:41:53 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/13 21:02:46 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	coder_take_dongles(t_coder *coder)
{
	t_simulation *sim = coder->sim;

	if (coder->id % 2 == 0)
	{
		take_dongle(coder->right_dongle, sim->config.dongle_cooldown);
		take_dongle(coder->left_dongle, sim->config.dongle_cooldown);
	}
	else
	{
		take_dongle(coder->left_dongle, sim->config.dongle_cooldown);
		take_dongle(coder->right_dongle, sim->config.dongle_cooldown);
	}
	log_event(sim, coder->id, "has taken a dongle");
	log_event(sim, coder->id, "has taken a dongle");
}

void	coder_compile(t_coder *coder)
{
	t_simulation *sim = coder->sim;

	coder->last_compile_start = get_current_time();
	log_event(sim, coder->id, "is compiling");
	usleep(sim->config.time_to_compile * 1000);
	release_dongle(coder->left_dongle);
	release_dongle(coder->right_dongle);
}

void	coder_debug_refactor(t_coder *coder)
{
	t_simulation *sim = coder->sim;

	log_event(sim, coder->id, "is debugging");
	usleep(sim->config.time_to_debug * 1000);
	log_event(sim, coder->id, "is refactoring");
	usleep(sim->config.time_to_refactor * 1000);
}

int	check_and_update_stop(t_coder *coder)
{
	t_simulation	*sim;
	int				i;
	int				done;

	sim = coder->sim;
	coder->compiles_done++;
	pthread_mutex_lock(&sim->stop_lock);
	i = 0;
	done = 1;
	while (i < sim->config.number_of_coders)
	{
		if (sim->coders[i].compiles_done < sim->config.number_of_compiles_required)
			done = 0;
		i++;
	}
	if (done)
		sim->stop = 1;
	i = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	return (i);
}

void	*coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;
	int				stop;

	coder = (t_coder *)arg;
	sim = coder->sim;
	pthread_mutex_lock(&sim->stop_lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	while (!stop)
	{
		coder_take_dongles(coder);
		coder_compile(coder);
		coder_debug_refactor(coder);
		stop = check_and_update_stop(coder);
	}
	return (NULL);
}