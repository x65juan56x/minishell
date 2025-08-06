/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expressions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:07 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:16 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*node;
	t_ast_node	*cmd_node;
	t_list		*arg_list;

	if (!parser->current || (parser->current->type != TOKEN_WORD
			&& !is_redirect_token(parser->current->type)))
		return (consume_token_type(parser, TOKEN_WORD), NULL);
	cmd_node = create_ast_node(NODE_COMMAND);
	if (!cmd_node)
		return (NULL);
	arg_list = NULL;
	node = cmd_node;
	if (process_tokens_loop(parser, &arg_list, &node) != 0)
		return (ft_lstclear(&arg_list, free), cleanup_ast(node), NULL);
	cmd_node->args = convert_arg_list_to_array(arg_list);
	ft_lstclear(&arg_list, free);
	if (!cmd_node->args)
		return (cleanup_ast(node), NULL);
	return (node);
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
