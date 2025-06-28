/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:31:15 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/12 14:43:41 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static int	wait_all(pid_t *pids, int count)
{
    int	i, status, last = 0;
    pid_t wpid;

    i = 0;
    while (i < count)
    {
        wpid = wait(&status);
        if (wpid == pids[count - 1])
        {
            if (WIFEXITED(status))
                last = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                last = 128 + WTERMSIG(status);
        }
        i++;
    }
    return (last);
}

static void	here_doc(char *limiter)
{
    int	pipefd[2];
    pid_t	pid;
    char	*line;

    if (pipe(pipefd) == -1)
        ft_perror_exit("pipe here_doc", EXIT_FAILURE);
    pid = fork();
    if (pid == -1)
        ft_perror_exit("fork here_doc", EXIT_FAILURE);
    if (pid == 0)
    {
        close(pipefd[0]);
        while (1)
        {
            ft_putstr_fd("heredoc> ", 1);
            if (ft_mini_gnl(&line) <= 0)
                break;
            if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
                && line[ft_strlen(limiter)] == '\n')
            {
                free(line);
                break;
            }
            write(pipefd[1], line, ft_strlen(line));
            free(line);
        }
        close(pipefd[1]);
        exit(0);
    }
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    waitpid(pid, NULL, 0);
}

static void	init_pipex(t_pipex *px, int ac, char **av)
{
    int	i;

    px->ncmds = ac - (ft_strncmp(av[1], "here_doc", 8) == 0 ? 4 : 3);
    px->pipes = malloc(sizeof(int *) * (px->ncmds - 1));
    i = 0;
    while (i < px->ncmds - 1)
    {
        px->pipes[i] = malloc(sizeof(int) * 2);
        pipe(px->pipes[i]);
        i++;
    }
}

static void	take_the_pipeline(int ac, char **av, char **envp)
{
    t_pipex	px;
    int		i;

    if (ft_strncmp(av[1], "here_doc", 8) == 0)
    {
        px.fd_out = open_mode(av[ac - 1], 0);
        here_doc(av[2]);
        px.fd_in = STDIN_FILENO;
    }
    else
    {
        px.fd_out = open_mode(av[ac - 1], 1);
        px.fd_in = open_mode(av[1], 2);
    }
    init_pipex(&px, ac, av);
    px.pids = ft_calloc(px.ncmds, sizeof(pid_t));
    i = 0;
    while (i < px.ncmds)
    {
        px.pids[i] = create_command_child(av[ac - px.ncmds - 1 + i], envp, &px, i);
        i++;
    }
    close_all_pipes(&px);
    exit(wait_all(px.pids, px.ncmds));
}

int	main(int ac, char **av, char **envp)
{
    if (ac < 5)
        wrong_args();
    take_the_pipeline(ac, av, envp);
    return (0);
}
