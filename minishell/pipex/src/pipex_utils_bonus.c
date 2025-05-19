/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:31:19 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/12 14:43:42 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

void	wrong_args(void)
{
	ft_putstr_fd("Error: Wrong arguments\n", 2);
	ft_putstr_fd("Ex: ./pipex <file1> <cmd1> <cmd2> <...> <file2>\n", 2);
	ft_putstr_fd("    ./pipex \"here_doc\" <LIMITER> <cmd1> ", 2);
	ft_putstr_fd("<cmd2> <...> <file>\n", 2);
	exit(EXIT_FAILURE);
}

int	open_mode(char *file, int i)
{
	int	fd;

	fd = 0;
	if (i == 0)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (i == 1)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (i == 2)
		fd = open(file, O_RDONLY);
	if (fd == -1)
		ft_perror_exit(file, EXIT_FAILURE);
	return (fd);
}
