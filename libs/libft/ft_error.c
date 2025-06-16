/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:30:41 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/12 14:43:33 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

void	ft_perror_exit(const char *prefix, int exit_code)
{
	if (prefix)
		perror(prefix);
	else
		perror("🐚 ");
	exit(exit_code);
}

void	ft_cmd_not_found_exit(const char *cmd_name)
{
	ft_putstr_fd("🐚: ", STDERR_FILENO);
	if (cmd_name && *cmd_name)
	{
		ft_putstr_fd((char *)cmd_name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putstr_fd("command not found\n", STDERR_FILENO);
	exit(127);
}

void	ft_permission_denied_exit(const char *cmd_name)
{
	ft_putstr_fd("🐚: ", STDERR_FILENO);
	if (cmd_name && *cmd_name)
	{
		ft_putstr_fd((char *)cmd_name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	perror(cmd_name);
	exit(126);
}

void	ft_error(void)
{
	perror("Error");
	exit(EXIT_FAILURE);
}
