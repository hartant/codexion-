/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 22:23:13 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/12 21:32:09 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long	get_current_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	log_event(t_simulation *sim, int coder_id, char *message)
{
	long	timestamp;

	timestamp = get_current_time() - sim->start_time;
	pthread_mutex_lock(&sim->print_lock);
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&sim->print_lock);
}