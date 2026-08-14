/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 21:12:43 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/14 18:57:00 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long	get_request_value(t_coder *coder)
{
	if (coder->sim->config.scheduler == fifo)
		return (get_current_time());
	return (coder->last_compile_start + coder->sim->config.time_to_burnout);
}

int	has_priority(t_scheduler sched, t_request a, t_request b)
{
	(void)sched;
	if (a.value != b.value)
		return (a.value < b.value);
	return (a.coder_id < b.coder_id);
}

void	heap_push(t_dongle *d, t_request req, t_scheduler sched)
{
	int			i;
	int			parent;
	t_request	tmp;

	d->waiting[d->waiting_count] = req;
	i = d->waiting_count;
	d->waiting_count++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!has_priority(sched, d->waiting[i], d->waiting[parent]))
			break ;
		tmp = d->waiting[i];
		d->waiting[i] = d->waiting[parent];
		d->waiting[parent] = tmp;
		i = parent;
	}
}

t_request	heap_pop(t_dongle *d, t_scheduler sched)
{
	t_request	top;
	t_request	tmp;
	int			i;
	int			left;
	int			right;
	int			smallest;

	top = d->waiting[0];
	d->waiting_count--;
	d->waiting[0] = d->waiting[d->waiting_count];
	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < d->waiting_count && has_priority(sched, d->waiting[left],
				d->waiting[smallest]))
			smallest = left;
		if (right < d->waiting_count && has_priority(sched, d->waiting[right],
				d->waiting[smallest]))
			smallest = right;
		if (smallest == i)
			break ;
		tmp = d->waiting[i];
		d->waiting[i] = d->waiting[smallest];
		d->waiting[smallest] = tmp;
		i = smallest;
	}
	return (top);
}
