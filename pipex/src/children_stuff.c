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

typedef void	(*t_child_fn)(char **av, int *fd_pipe, char **envp);

void	close_all_pipes(t_pipex *px)
{
    int	i;

    i = 0;
    while (i < px->ncmds - 1)
    {
        close(px->pipes[i][0]);
        close(px->pipes[i][1]);
        i++;
    }
}

void	close_unused_pipes(t_pipex *px, int idx)
{
    int	i;

    i = 0;
    while (i < px->ncmds - 1)
    {
        if (i != idx - 1)
            close(px->pipes[i][0]);
        if (i != idx)
            close(px->pipes[i][1]);
        i++;
    }
}

// Función genérica para crear un hijo con callback (mandatorio)
pid_t	create_child_with_fn(char **av, int *fd_pipe, char **envp, t_child_fn fn)
{
    pid_t	pid;

    pid = fork();
    if (pid == -1)
    {
        close(fd_pipe[0]);
        close(fd_pipe[1]);
        ft_perror_exit("fork_child", EXIT_FAILURE);
    }
    if (pid == 0)
        fn(av, fd_pipe, envp);
    return (pid);
}

// Hijo 1 (mandatorio)
static void	run_child_p_one(char **av, int *fd_pipe, char **envp)
{
    int	infile_fd;

    close(fd_pipe[0]);
    if (dup2(fd_pipe[1], STDOUT_FILENO) == -1)
        ft_perror_exit("dup2_pipe_stdout", EXIT_FAILURE);
    close(fd_pipe[1]);
    infile_fd = open(av[1], O_RDONLY);
    if (infile_fd == -1)
    {
        ft_perror_exit(av[1], 1);
        exit(1);
    }
    if (dup2(infile_fd, STDIN_FILENO) == -1)
        ft_perror_exit("dup2_infile_stdin", EXIT_FAILURE);
    close(infile_fd);
    run_cmd(av[2], envp);
}

// Hijo 2 (mandatorio)
static void	run_child_p_two(char **av, int *fd_pipe, char **envp)
{
    int	outfile_fd;

    close(fd_pipe[1]);
    if (dup2(fd_pipe[0], STDIN_FILENO) == -1)
        ft_perror_exit("dup2_pipe_stdin", EXIT_FAILURE);
    close(fd_pipe[0]);
    outfile_fd = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfile_fd == -1)
        ft_perror_exit(av[4], EXIT_FAILURE);
    if (dup2(outfile_fd, STDOUT_FILENO) == -1)
        ft_perror_exit("dup2_outfile_stdout", EXIT_FAILURE);
    close(outfile_fd);
    run_cmd(av[3], envp);
}

// Wrapper para el primer hijo (mandatorio)
pid_t	create_child_one(char **av, int *fd_pipe, char **envp)
{
    return create_child_with_fn(av, fd_pipe, envp, run_child_p_one);
}

// Wrapper para el segundo hijo (mandatorio)
pid_t	create_child_two(char **av, int *fd_pipe, char **envp)
{
    return create_child_with_fn(av, fd_pipe, envp, run_child_p_two);
}

// Función genérica para el bonus (o pipelines de N comandos)
pid_t	create_command_child(char *cmd, char **envp, t_pipex *px, int idx)
{
    pid_t	pid;

    pid = fork();
    if (pid == -1)
        ft_perror_exit("fork", EXIT_FAILURE);
    if (pid == 0)
    {
        if (idx == 0 && px->fd_in != STDIN_FILENO)
        {
            dup2(px->fd_in, STDIN_FILENO);
            close(px->fd_in);
        }
        else if (idx > 0)
            dup2(px->pipes[idx - 1][0], STDIN_FILENO);
        if (idx == px->ncmds - 1 && px->fd_out != STDOUT_FILENO)
        {
            dup2(px->fd_out, STDOUT_FILENO);
            close(px->fd_out);
        }
        else if (idx < px->ncmds - 1)
            dup2(px->pipes[idx][1], STDOUT_FILENO);
        close_unused_pipes(px, idx);
        run_cmd(cmd, envp);
    }
    return (pid);
}
