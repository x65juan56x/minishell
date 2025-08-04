#include "../../include/minishell.h"

static void	print_ueof_message(char **input_ptr)
{
	ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
	free(*input_ptr);
	*input_ptr = ft_strdup("");
}

static int	handle_multiline_input(char **input_ptr)
{
	char	*full_input;
	char	*temp;
	char	*new_input;

	while (are_quotes_unclosed(*input_ptr)
		|| are_parentheses_unclosed(*input_ptr))
	{
		if (!isatty(STDIN_FILENO))
			return (print_ueof_message(input_ptr), 0);
		full_input = readline("> ");
		if (!full_input)
			return (print_ueof_message(input_ptr), 0);
		temp = ft_strjoin(*input_ptr, " ");
		new_input = ft_strjoin(temp, full_input);
		free(temp);
		free(full_input);
		free(*input_ptr);
		*input_ptr = new_input;
		if (!*input_ptr)
			return (1);
	}
	return (0);
}

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
//	ft_putendl_fd("exit", STDOUT_FILENO); DESCOMENTAR!! NO BORRAR
	exit_code = get_exit_status_from_args(tokens->next);
	shell_context->should_exit = 1;
	shell_context->exit_status = exit_code;
	return (exit_code);
}

static void	process_input(char *input, t_shell_context *shell_context)
{
	t_token	*tokens;
	int		exit_code;

	if (!*input)
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

	while (1)
	{
		setup_interactive_signals();
		g_signal_status = 0;
		input = get_user_input();
		if (!input)
			break ;
		multiline_status = handle_multiline_input(&input);
		if (multiline_status == 1)
			return (1);
		if (g_signal_status == SIGINT)
		{
			shell_context->exit_status = 130;
			free(input);
			continue ;
		}
		process_input(input, shell_context);
		free(input);
		if (shell_context->should_exit)
			break ;
	}
	return (shell_context->exit_status);
}
