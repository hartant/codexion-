/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 18:19:49 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/10 22:14:18 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

typedef enum e_scheduler
{
    fifo,
    edf
}   t_scheduler;

typedef struct s_config
{
    int             number_of_coders;
    long            time_to_burnout;
    long            time_to_compile;
    long            time_to_debug;
    long            time_to_refactor;
    int             number_of_compiles_required;
    long            dongle_cooldown;
    t_scheduler     scheduler;
}   t_config;

typedef struct s_dongle
{
    int             id;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int             available;
    long            last_release_time;
}   t_dongle;

typedef struct s_coder
{
    int                 id;
    pthread_t           thread;
    t_dongle            *left_dongle;
    t_dongle            *right_dongle;
    long                last_compile_start;
    int                 compiles_done;
    struct s_simulation *sim;
}   t_coder;

typedef struct s_simulation
{
    t_config        config;
    t_coder         *coders;
    t_dongle        *dongles;

    pthread_mutex_t print_lock;
    pthread_mutex_t stop_lock;
    int             stop;

    long            start_time;
}   t_simulation;

int	is_valid_number(char *str);
t_config	parse_args(int argc, char **argv);
long	ft_atoll(char *str);
int	is_diigit(char *str);


#endif