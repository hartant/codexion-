/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 00:02:26 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/13 17:43:12 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"



void take_dongle(t_dongle *dongle, long cooldown)
{
    pthread_mutex_lock(&dongle->lock);
      
    while(!dongle->available || (dongle->last_release_time + cooldown > get_current_time()))
    {
        pthread_cond_wait(&dongle->cond, &dongle->lock);
    }
    
    dongle->available = 0;
    pthread_mutex_unlock(&dongle->lock);
}

void	release_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&dongle->lock);
    dongle->available = 1;
    dongle->last_release_time = get_current_time();
    pthread_cond_signal(&dongle->cond);
    pthread_mutex_unlock(&dongle->lock);
}