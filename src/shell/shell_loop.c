#include "../../include/minishell.h"

static int	check_for_exit_command(t_token *tokens,
				t_shell_context *shell_context)
{
	t_token	*current;
	int		exit_code;

	if (tokens->type != TOKEN_WORD || ft_strcmp(tokens->value, "exit") != 0)
		return (-1);
	current = tokens->next;
	while (current && current->type == TOKEN_WORD)
		current = current->next;
	if (current && current->type != TOKEN_EOF)
		return (-1);
//	ft_putendl_fd("exit", STDOUT_FILENO);
	exit_code = get_exit_status_from_args(tokens->next);
	shell_context->should_exit = 1;
	shell_context->exit_status = exit_code;
	return (exit_code);
}

static int	is_only_whitespace(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
			return (0);
		i++;
	}
	return (1);
}

static void	process_input(char *input, t_shell_context *shell_context)
{
	t_token	*tokens;
	int		exit_code;

	if (!*input || is_only_whitespace(input))
		return ;
	handle_input_line(input);
	tokens = tokenize(input);
	if (!tokens)
	{
		shell_context->exit_status = 2;
		return ;
	}
	if (check_for_exit_command(tokens, shell_context) == -1)
	{
		exit_code = process_command_line(tokens, shell_context);
		shell_context->exit_status = exit_code;
	}
	else
		cleanup_tokens(tokens);
}

int	run_shell_loop(t_shell_context *shell_context)
{
	char	*input;
	int		multiline_status;
	char	*cleanup_line;

	while (1)
	{
		setup_interactive_signals();
		g_signal_status = 0;
		input = get_user_input();
		if (!input)
			break ;
		multiline_status = handle_multiline_input(&input);
		if (g_signal_status == SIGINT)
			shell_context->exit_status = 130;
		if (multiline_status == 1)
			return (free(input), 1);
		if (multiline_status == 2)
			free(input);
		if (multiline_status == 2)
			continue ;
		process_input(input, shell_context);
		free(input);
		if (shell_context->should_exit)
			break ;
		if (!isatty(STDIN_FILENO))
		{
			cleanup_line = get_next_line(STDIN_FILENO);
			if (cleanup_line)
				free(cleanup_line);
			break ;
		}
	}
	return (shell_context->exit_status);
}
