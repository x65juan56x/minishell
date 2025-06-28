/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 20:51:30 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/03 20:51:45 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*p;
	size_t	s;

	s = nmemb * size;
	p = malloc(s);
	if (p == NULL)
		return (NULL);
	ft_bzero(p, s);
	return (p);
}
