/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbenamar <mbenamar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 21:20:00 by mbenamar          #+#    #+#             */
/*   Updated: 2026/08/11 21:21:12 by mbenamar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	is_digit(char *str)
{
	while (*str)
	{
		if (!(*str >= '0' && *str <= '9'))
			return (0);
		str++;
	}
	return (1);
}

long	ft_atoll(char *str)
{
	long	res;
	int		i;

	res = 0;
	i = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return (res);
}

void	check_digits(char *str)
{
	if (!is_digit(str))
	{
		printf("ERROR: argument is not a valid number\n");
		exit(1);
	}
}

void	check_positive(char *str)
{
	check_digits(str);
	if (ft_atoll(str) <= 0)
	{
		printf("ERROR: argument must be a positive number\n");
		exit(1);
	}
}
