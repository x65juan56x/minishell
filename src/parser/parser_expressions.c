#include "../../include/minishell.h"

t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*node;
	t_ast_node	*cmd_node;
	char		**args;
	int			arg_count;

	if (parser->current && parser->current->type != TOKEN_WORD
		&& !is_redirect_token(parser->current->type))
		return (consume_token_type(parser, TOKEN_WORD), NULL);
	cmd_node = create_ast_node(NODE_COMMAND);
	if (!cmd_node)
		return (NULL);
	args = ft_calloc(1024, sizeof(char *));
	if (!args)
		return (cleanup_ast(cmd_node), NULL);
	arg_count = 0;
	node = cmd_node;
	while (parser->current && (parser->current->type == TOKEN_WORD
			|| is_redirect_token(parser->current->type)))
		if (parse_args_and_redirect(&node, args, &arg_count, parser) != 0)
			return (NULL);
	cmd_node->args = create_compact_args(args, &arg_count);
	if (!cmd_node->args)
		return (ft_freearr(args), cleanup_ast(node), NULL);
	return (ft_freearr(args), node);
}

t_ast_node	*parse_parenthesis_expression(t_parser *parser)
{
	t_ast_node	*expr;

	if (!consume_token_type(parser, TOKEN_LPAREN))
		return (NULL);
	expr = parse_logical_expression(parser);
	if (!expr)
		return (NULL);
	if (!consume_token_type(parser, TOKEN_RPAREN))
		return (cleanup_ast(expr), NULL);
	return (expr);
}

t_ast_node	*parse_primary_expression(t_parser *parser)
{
	if (parser->current && parser->current->type == TOKEN_LPAREN)
		return (parse_parenthesis_expression(parser));
	return (parse_redirect_expression(parser));
}

t_ast_node	*parse_pipe_expression(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*new_node;
	t_token		*pipe_token;

	left = parse_primary_expression(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == TOKEN_PIPE)
	{
		pipe_token = consume_token_type(parser, TOKEN_PIPE);
		if (!pipe_token)
			return (cleanup_ast(left), NULL);
		right = parse_primary_expression(parser);
		if (!right)
			return (cleanup_ast(left), NULL);
		new_node = create_binary_node(TOKEN_PIPE, left, right);
		if (!new_node)
			return (cleanup_ast(left), cleanup_ast(right), NULL);
		left = new_node;
	}
	return (left);
}

t_ast_node	*parse_logical_expression(t_parser *parser)
{
	t_ast_node		*left;
	t_ast_node		*right;
	t_ast_node		*new_node;
	t_token_type	op_type;

	left = parse_pipe_expression(parser);
	if (!left)
		return (NULL);
	while (parser->current && (parser->current->type == TOKEN_AND
			|| parser->current->type == TOKEN_OR))
	{
		op_type = parser->current->type;
		consume_token_type(parser, op_type);
		if (parser->error)
			return (cleanup_ast(left), NULL);
		right = parse_pipe_expression(parser);
		if (!right)
			return (cleanup_ast(left), NULL);
		new_node = create_binary_node(op_type, left, right);
		if (!new_node)
			return (cleanup_ast(left), cleanup_ast(right), NULL);
		left = new_node;
	}
	return (left);
}
