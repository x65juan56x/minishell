#include "../../include/minishell.h"

static void	print_ueof_message(void)
{
	ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
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
		{
			print_ueof_message();
			return (free(*input_ptr), *input_ptr = ft_strdup(""), 0);
		}
		full_input = readline("> ");
		if (!full_input)
			return (print_ueof_message(), -1);
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

static int	check_for_exit_command(t_token *tokens)
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
	exit_code = get_exit_status_from_args(tokens->next);
	cleanup_tokens(tokens);
	exit(exit_code);
}
//	ft_putendl_fd("exit", STDOUT_FILENO);

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
	if (check_for_exit_command(tokens) == -1)
	{
		exit_code = process_command_line(tokens, shell_context);
		shell_context->exit_status = exit_code;
	}
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
		if (multiline_status == -1)
		{
			free(input);
			input = ft_strdup("");
		}
		else if (multiline_status == 1)
			return (1);
		if (g_signal_status == SIGINT)
			shell_context->exit_status = 130;
		process_input(input, shell_context);
		free(input);
	}
	return (shell_context->exit_status);
}
