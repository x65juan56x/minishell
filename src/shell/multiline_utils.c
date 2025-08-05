#include "../../include/minishell.h"

void	cancel_multiline_input(char **input_ptr)
{
	free(*input_ptr);
	*input_ptr = ft_strdup("");
}

int	needs_multiline(char *input)
{
	return (are_quotes_unclosed(input)
		|| are_parentheses_unclosed(input)
		|| (!are_quotes_unclosed(input)
			&& has_pending_logical_operator(input)));
}

int	should_handle_logical_ops(char *input)
{
	return (!are_quotes_unclosed(input)
		&& !are_parentheses_unclosed(input)
		&& has_pending_logical_operator(input));
}
