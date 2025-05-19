/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:31:29 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/14 16:13:30 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static int	wait_children_get_exit(pid_t pid_one, pid_t pid_two)
{
	int	status_one;
	int	status_two;

	waitpid(pid_one, &status_one, 0);
	waitpid(pid_two, &status_two, 0);
	if (WIFEXITED(status_two))
		return (WEXITSTATUS(status_two));
	if (WIFSIGNALED(status_two))
		return (128 + WTERMSIG(status_two));
	if (WIFEXITED(status_one))
		return (WEXITSTATUS(status_one));
	if (WIFSIGNALED(status_one))
		return (128 + WTERMSIG(status_one));
	return (EXIT_FAILURE);
}

int	main(int ac, char **av, char **envp)
{
	int		fd_pipe[2];
	pid_t	pid_one;
	pid_t	pid_two;
	int		final_exit_status;

	if (ac != 5)
	{
		ft_putstr_fd("pipex: Error: Wrong arguments\n", STDERR_FILENO);
		ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 outfile\n",
			STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
	if (pipe(fd_pipe) == -1)
		ft_perror_exit("pipe", EXIT_FAILURE);
	pid_one = create_child_one(av, fd_pipe, envp);
	pid_two = create_child_two(av, fd_pipe, envp, pid_one);
	close(fd_pipe[0]);
	close(fd_pipe[1]);
	final_exit_status = wait_children_get_exit(pid_one, pid_two);
	exit(final_exit_status);
}
