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
# include <time.h>

typedef enum e_scheduler
{
    fifo,
    edf
}   t_scheduler;

typedef struct s_request
{
	int		coder_id;
	long	value;
}	t_request;

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
	int				id;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	int				available;
	long			last_release_time;
	t_request		*waiting;
	int				waiting_count;
}	t_dongle;
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


int             is_digit(char *str);
long            ft_atoll(char *str);
void            check_digits(char *str);
void            check_positive(char *str);
t_scheduler     parse_scheduler(char *str);
void            fill_config(t_config *config, char **argv);
t_config        parse_args(char **argv);
t_simulation	*init_simulation(t_config config);
long	get_current_time(void);
void	log_event(t_simulation *sim, int coder_id, char *message);
void	take_dongle(t_dongle *dongle, long cooldown);
void	release_dongle(t_dongle *dongle);
void	coder_take_dongles(t_coder *coder);
void	coder_compile(t_coder *coder);
void	coder_debug_refactor(t_coder *coder);
int		check_and_update_stop(t_coder *coder);
void	*coder_routine(void *arg);
int		create_threads(t_simulation *sim);
void	join_threads(t_simulation *sim);
long	get_current_time(void);
void    cleanup_simulation(t_simulation *sim);
int			has_priority(t_scheduler sched, t_request a, t_request b);
void		heap_push(t_dongle *d, t_request req, t_scheduler sched);
t_request	heap_pop(t_dongle *d, t_scheduler sched);


#endif