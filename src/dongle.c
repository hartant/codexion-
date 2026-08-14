/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 00:02:26 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/13 22:04:27 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request		req;
	long			cooldown;
	t_scheduler		sched;
	struct timespec	ts;
	long			deadline;
	int				stop;

	cooldown = coder->sim->config.dongle_cooldown;
	sched = coder->sim->config.scheduler;
	pthread_mutex_lock(&dongle->lock);
	req.coder_id = coder->id;
	req.value = get_request_value(coder);
	heap_push(dongle, req, sched);
	pthread_mutex_lock(&coder->sim->stop_lock);
	stop = coder->sim->stop;
	pthread_mutex_unlock(&coder->sim->stop_lock);
	while (!stop && (dongle->waiting[0].coder_id != coder->id
			|| !dongle->available || dongle->last_release_time
			+ cooldown > get_current_time()))
	{
		deadline = dongle->last_release_time + cooldown;
		ts.tv_sec = deadline / 1000;
		ts.tv_nsec = (deadline % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		pthread_mutex_lock(&coder->sim->stop_lock);
		stop = coder->sim->stop;
		pthread_mutex_unlock(&coder->sim->stop_lock);
	}
	if (stop)
	{
		pthread_mutex_unlock(&dongle->lock);
		return (0);
	}
	heap_pop(dongle, sched);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->lock);
	return (1);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->available = 1;
	dongle->last_release_time = get_current_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}