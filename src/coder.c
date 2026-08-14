/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 18:41:53 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/14 18:52:36 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	coder_take_dongles(t_coder *coder)
{
	t_simulation	*sim;
	t_dongle		*first;
	t_dongle		*second;

	sim = coder->sim;
	if (coder->id % 2 == 0)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	else
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	if (!take_dongle(first, coder))
		return (0);
	log_event(sim, coder->id, "has taken a dongle");
	if (!take_dongle(second, coder))
	{
		release_dongle(first);
		return (0);
	}
	log_event(sim, coder->id, "has taken a dongle");
	return (1);
}

int	coder_compile(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->stop_lock);
	coder->last_compile_start = get_current_time();
	pthread_mutex_unlock(&sim->stop_lock);
	log_event(sim, coder->id, "is compiling");
	if (!interruptible_sleep(sim, sim->config.time_to_compile))
	{
		release_dongle(coder->left_dongle);
		release_dongle(coder->right_dongle);
		return (0);
	}
	release_dongle(coder->left_dongle);
	release_dongle(coder->right_dongle);
	return (1);
}

int	coder_debug_refactor(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	log_event(sim, coder->id, "is debugging");
	if (!interruptible_sleep(sim, sim->config.time_to_debug))
		return (0);
	log_event(sim, coder->id, "is refactoring");
	if (!interruptible_sleep(sim, sim->config.time_to_refactor))
		return (0);
	return (1);
}

int	check_and_update_stop(t_coder *coder)
{
	t_simulation	*sim;
	int				i;
	int				done;

	sim = coder->sim;
	pthread_mutex_lock(&sim->stop_lock);
	coder->compiles_done++;
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
		if (!coder_take_dongles(coder))
			break ;
		if (!coder_compile(coder))
			break ;
		if (!coder_debug_refactor(coder))
			break ;
		stop = check_and_update_stop(coder);
	}
	return (NULL);
}
