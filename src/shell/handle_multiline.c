#include "../../include/minishell.h"

static void	print_ueof_message(char **input_ptr)
{
	ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
	free(*input_ptr);
	*input_ptr = ft_strdup("");
}

static int	needs_multiline(char *input)
{
	return (are_quotes_unclosed(input)
		|| are_parentheses_unclosed(input)
		|| (!are_quotes_unclosed(input)
			&& has_pending_logical_operator(input)));
}

static int	should_handle_logical_ops(char *input)
{
	return (!are_quotes_unclosed(input)
		&& !are_parentheses_unclosed(input)
		&& has_pending_logical_operator(input));
}

static int	append_input_line(char **input_ptr, char *new_line)
{
	char	*temp;
	char	*new_input;

	temp = ft_strjoin(*input_ptr, " ");
	new_input = ft_strjoin(temp, new_line);
	free(temp);
	free(new_line);
	free(*input_ptr);
	*input_ptr = new_input;
	if (!*input_ptr)
		return (1);
	return (0);
}

int	handle_multiline_input(char **input_ptr)
{
	char	*full_input;

	while (needs_multiline(*input_ptr))
	{
		if (!isatty(STDIN_FILENO))
		{
			if (should_handle_logical_ops(*input_ptr))
			{
				full_input = get_next_line(STDIN_FILENO);
				if (!full_input)
					return (print_ueof_message(input_ptr), 0);
				if (append_input_line(input_ptr, full_input))
					return (1);
				continue ;
			}
			return (print_ueof_message(input_ptr), 0);
		}
		full_input = readline("> ");
		if (!full_input)
			return (print_ueof_message(input_ptr), 0);
		if (append_input_line(input_ptr, full_input))
			return (1);
	}
	return (0);
}
