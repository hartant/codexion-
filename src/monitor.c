/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 21:35:57 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/14 18:56:02 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	check_coder_burnout(t_coder *coder)
{
	long	elapsed;
	long	burnout;

	pthread_mutex_lock(&coder->sim->stop_lock);
	elapsed = get_current_time() - coder->last_compile_start;
	burnout = coder->sim->config.time_to_burnout;
	pthread_mutex_unlock(&coder->sim->stop_lock);
	if (elapsed >= burnout)
		return (1);
	return (0);
}

int	check_all_burnout(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (check_coder_burnout(&sim->coders[i]))
		{
			log_event(sim, sim->coders[i].id, "burned out");
			pthread_mutex_lock(&sim->stop_lock);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->stop_lock);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int		stop;

	sim = (t_simulation *)arg;
	pthread_mutex_lock(&sim->stop_lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	while (!stop)
	{
		if (check_all_burnout(sim))
			break ;
		usleep(2000);
		pthread_mutex_lock(&sim->stop_lock);
		stop = sim->stop;
		pthread_mutex_unlock(&sim->stop_lock);
	}
	return (NULL);
}
