/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:20:51 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:35:12 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*get_user_input(void)
{
	char	*input;

	if (isatty(STDIN_FILENO))
	{
		input = readline(CORAL_BOLD "MiniShell" RESET_COLOR " $ ");
		if (!input)
			return (ft_putstr_fd("exit\n", STDOUT_FILENO), NULL);
	}
	else
		input = get_next_line(STDIN_FILENO);
	return (input);
}

int	handle_input_line(char *input)
{
	if (!input)
		return (0);
	if (*input)
		if (isatty(STDIN_FILENO))
			add_history(input);
	return (0);
}

int	get_exit_status_from_args(t_token *args_token)
{
	if (!args_token || args_token->type == TOKEN_EOF)
		return (0);
	if (!ft_isnumstr(args_token->value))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args_token->value, STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		return (2);
	}
	if (args_token->next && args_token->next->type != TOKEN_EOF)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	return ((unsigned char)ft_atoi(args_token->value));
}

int	check_noisatty(void)
{
	char	*cleanup_line;

	if (!isatty(STDIN_FILENO))
	{
		cleanup_line = get_next_line(STDIN_FILENO);
		if (cleanup_line)
			free(cleanup_line);
		return (1);
	}
	return (0);
}

void	check_sigint(t_shell_context *shell_context)
{
	if (g_signal_status == SIGINT)
		shell_context->exit_status = 130;
	return ;
}
