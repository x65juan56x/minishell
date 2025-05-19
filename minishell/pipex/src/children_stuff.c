/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   children_stuff.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 12:45:56 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/14 17:34:27 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static void	run_child_p_one(char *infile_path, char *cmd_str, int *pipe_fds,
				char **envp)
{
	int	infile_fd;

	close(pipe_fds[0]);
	if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
		ft_perror_exit("dup2_pipe_stdout", EXIT_FAILURE);
	close(pipe_fds[1]);
	infile_fd = open(infile_path, O_RDONLY);
	if (infile_fd == -1)
	{
		ft_perror_exit(infile_path, 1);
		close(pipe_fds[1]);
		exit(1);
	}
	if (dup2(infile_fd, STDIN_FILENO) == -1)
		ft_perror_exit("dup2_infile_stdin", EXIT_FAILURE);
	close(infile_fd);
	run_cmd(cmd_str, envp);
}

static void	run_child_p_two(char *outfile_path, char *cmd_str,
				int *pipe_fds, char **envp)
{
	int	outfile_fd;

	close(pipe_fds[1]);
	if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
		ft_perror_exit("dup2_pipe_stdin", EXIT_FAILURE);
	close(pipe_fds[0]);
	outfile_fd = open(outfile_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile_fd == -1)
		ft_perror_exit(outfile_path, EXIT_FAILURE);
	if (dup2(outfile_fd, STDOUT_FILENO) == -1)
		ft_perror_exit("dup2_outfile_stdout", EXIT_FAILURE);
	close(outfile_fd);
	run_cmd(cmd_str, envp);
}

pid_t	create_child_one(char **av, int *fd_pipe, char **envp)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		close(fd_pipe[0]);
		close(fd_pipe[1]);
		ft_perror_exit("fork_child1", EXIT_FAILURE);
	}
	if (pid == 0)
		run_child_p_one(av[1], av[2], fd_pipe, envp);
	return (pid);
}

pid_t	create_child_two(char **av, int *fd_pipe, char **envp, pid_t pid_one)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		close(fd_pipe[0]);
		close(fd_pipe[1]);
		if (pid_one > 0)
			waitpid(pid_one, NULL, 0);
		ft_perror_exit("fork_child2", EXIT_FAILURE);
	}
	if (pid == 0)
		run_child_p_two(av[4], av[3], fd_pipe, envp);
	return (pid);
}
