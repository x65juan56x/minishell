/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:23 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:37:02 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	heredoc_child_routine(const char *filename, char *delimiter,
			t_shell_context *shell_context)
{
	int	result;

	setup_heredoc_signals();
	result = create_heredoc_file(filename, delimiter, shell_context);
	cleanup_child_process_deep(shell_context);
	if (result != 0)
		exit(1);
	exit(0);
}

static int	heredoc_parent_routine(pid_t pid, struct termios *orig_termios)
{
	int	status;

	waitpid(pid, &status, 0);
	tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
	if (WIFSIGNALED(status) || (WIFEXITED(status)
			&& WEXITSTATUS(status) == 130))
	{
		g_signal_status = SIGINT;
		write(STDOUT_FILENO, "\n", 1);
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		return (-1);
	return (0);
}

int	execute_heredoc(const char *filename, char *delimiter,
	t_shell_context *shell_context)
{
	pid_t			pid;
	struct termios	orig_termios;

	tcgetattr(STDIN_FILENO, &orig_termios);
	ignore_signals();
	pid = fork();
	if (pid == -1)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		heredoc_child_routine(filename, delimiter, shell_context);
	return (heredoc_parent_routine(pid, &orig_termios));
}
