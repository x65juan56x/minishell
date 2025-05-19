/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:31:15 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/14 17:41:57 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static void	p_child(char *av, char **envp)
{
	pid_t	pid_c;
	int		fd[2];

	if (pipe(fd) == -1)
		ft_perror_exit("pipe", EXIT_FAILURE);
	pid_c = fork();
	if (pid_c == -1)
	{
		close(fd[0]);
		close(fd[1]);
		ft_perror_exit("fork", EXIT_FAILURE);
	}
	if (pid_c == 0)
	{
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1)
			ft_perror_exit("dup2", EXIT_FAILURE);
		close(fd[1]);
		run_cmd(av, envp);
	}
	close(fd[1]);
	if (dup2(fd[0], STDIN_FILENO) == -1)
		ft_perror_exit("dup2", EXIT_FAILURE);
	close(fd[0]);
}

static void	hd_child(int *fd, char *limiter)
{
	char	*line;

	line = NULL;
	close(fd[0]);
	while (ft_mini_gnl(&line) > 0)
	{
		if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
			&& (line[ft_strlen(limiter)] == '\0'
				|| line[ft_strlen(limiter)] == '\n'))
		{
			free(line);
			exit(EXIT_SUCCESS);
		}
		write(fd[1], line, ft_strlen(line));
		free(line);
		line = NULL;
	}
	if (line)
		free(line);
	exit(EXIT_FAILURE);
}

static void	here_doc(char *limiter, int ac)
{
	pid_t	pid_hd;
	int		fd[2];

	if (ac < 6)
		wrong_args();
	if (pipe(fd) == -1)
		ft_perror_exit("pipe", EXIT_FAILURE);
	pid_hd = fork();
	if (pid_hd == -1)
	{
		close(fd[0]);
		close(fd[1]);
		ft_perror_exit("fork", EXIT_FAILURE);
	}
	if (pid_hd == 0)
		hd_child(fd, limiter);
	else
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		waitpid(pid_hd, NULL, 0);
	}
}

static void	take_the_pipeline(int ac, char **av, char **envp)
{
	int	i;
	int	infile;
	int	outfile;

	if (ft_strncmp(av[1], "here_doc", 8) == 0)
	{
		i = 3;
		outfile = open_mode(av[ac - 1], 0);
		here_doc(av[2], ac);
	}
	else
	{
		i = 2;
		outfile = open_mode(av[ac - 1], 1);
		infile = open_mode(av[1], 2);
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	while (i < ac - 2)
		p_child(av[i++], envp);
	dup2(outfile, STDOUT_FILENO);
	close(outfile);
	run_cmd(av[ac - 2], envp);
}

int	main(int ac, char **av, char **envp)
{
	if (ac < 5)
		wrong_args();
	take_the_pipeline(ac, av, envp);
	return (0);
}
