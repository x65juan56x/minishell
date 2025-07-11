/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mini_gnl_char.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:30:53 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/12 14:43:37 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_mini_gnl_char(int fd)
{
	char	*buffer;
	int		i;
	int		r;
	char	c;

	i = 0;
	r = 0;
	buffer = (char *)malloc(10000);
	if (!buffer)
		return (NULL);
	r = read(fd, &c, 1);
	while (r && c != '\n' && c != '\0')
	{
		buffer[i++] = c;
		r = read(fd, &c, 1);
	}
	if (i > 0 || r > 0)
	{
		buffer[i] = '\0';
		return (buffer);
	}
	free(buffer);
	return (NULL);
}
