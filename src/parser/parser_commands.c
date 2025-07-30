#include "../../include/minishell.h"

t_token	*consume_token(t_parser *parser, t_token_type expected)
{
	t_token	*token;

	if (!parser->current || parser->current->type != expected)
	{
		if (!parser->error)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
			if (parser->current && parser->current->type != TOKEN_EOF)
				ft_putstr_fd(parser->current->value, STDERR_FILENO);
			else
				ft_putstr_fd("newline", STDERR_FILENO);
			ft_putstr_fd("'\n", STDERR_FILENO);
		}
		parser->error = 1;
		return (NULL);
	}
	token = parser->current;
	parser->current = parser->current->next;
	return (token);
}
