/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:12 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:26 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	match_wildcard(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '*')
	{
		if (*(pattern + 1) == '\0')
			return (1);
		while (*str)
		{
			if (match_wildcard(str, pattern + 1))
				return (1);
			str++;
		}
		return (match_wildcard(str, pattern + 1));
	}
	if (*str == *pattern)
		return (match_wildcard(str + 1, pattern + 1));
	return (0);
}
