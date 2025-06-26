#include "../../include/minishell.h"

static int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*new_node;
	t_token		*redirect_token;
	t_token		*file_token;

	left = parse_command(parser);
	if (!left)
		return (NULL);
	while (parser->current && is_redirect_token(parser->current->type))
	{
		redirect_token = parser->current;
		parser->current = parser->current->next;
		file_token = consume_token(parser, TOKEN_WORD);
		if (!file_token)
		{
			cleanup_ast(left);
			return (NULL);
		}
		new_node = create_redirect_node(redirect_token->type, left, file_token->value);
		if (!new_node)
			return (cleanup_ast(left), NULL);
		left = new_node;
	}
	return (left);
}

/*
La variable temporal "new_node" se crea para que en el caso de fallar "create_direct_node" podamos liberar "left"
*/

static t_token	*consume_token_type(t_parser *parser, t_token_type type)
{
	t_token	*token;

	if (!parser->current || parser->current->type != type)
	{
		parser->error = 1;
		return (NULL);
	}
	token = parser->current;
	parser->current = parser->current->next;
	return (token);
}

t_ast_node	*parse_pipe_expression(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*new_node;
	t_token		*pipe_token;

	left = parse_redirect_expression(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == TOKEN_PIPE)
	{
		pipe_token = consume_token_type(parser, TOKEN_PIPE);
		if (!pipe_token)
			return (cleanup_ast(left), NULL);
		right = parse_redirect_expression(parser);
		if (!right)
			return (cleanup_ast(left), NULL);
		new_node = create_binary_node(TOKEN_PIPE, left, right);
		if (!left)
			return (cleanup_ast(left), cleanup_ast(right), NULL);
		left = new_node;
	}
	return (left);
}
/*
La variable temporal "new_node" se crea para que en el caso de fallar "create_binary_node" podamos liberar "left"
*/
