/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 00:02:26 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/13 21:33:55 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"



#include "../include/codexion.h"

void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request		req;
	long			cooldown;
	t_scheduler		sched;

	cooldown = coder->sim->config.dongle_cooldown;
	sched = coder->sim->config.scheduler;
	pthread_mutex_lock(&dongle->lock);
	req.coder_id = coder->id;
	req.value = get_request_value(coder);
	heap_push(dongle, req, sched);
	while (dongle->waiting[0].coder_id != coder->id
		|| !dongle->available
		|| dongle->last_release_time + cooldown > get_current_time())
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	heap_pop(dongle, sched);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->lock);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->available = 1;
	dongle->last_release_time = get_current_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}