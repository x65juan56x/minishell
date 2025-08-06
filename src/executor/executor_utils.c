/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:11 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:36:56 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	print_signal_message(int signal_num)
{
	if (signal_num == SIGQUIT)
		ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
	else if (signal_num == SIGINT)
		ft_putstr_fd("\n", STDERR_FILENO);
}

int	analyze_child_status(int status)
{
	int	sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			g_signal_status = SIGINT;
		print_signal_message(sig);
		return (128 + sig);
	}
	return (status);
}

void	cleanup_heredoc_files(t_shell_context *shell_context)
{
	t_list	*current;
	t_list	*next;

	if (!shell_context || !shell_context->heredoc_files)
		return ;
	current = shell_context->heredoc_files;
	while (current)
	{
		next = current->next;
		unlink((char *)current->content);
		free(current->content);
		free(current);
		current = next;
	}
	shell_context->heredoc_files = NULL;
}
