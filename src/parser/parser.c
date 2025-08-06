/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:44 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:21 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*parse(t_token *tokens, t_shell_context *shell_context)
{
	t_parser	parser;
	t_ast_node	*ast;

	if (!tokens)
		return (NULL);
	parser.tokens = tokens;
	parser.current = tokens;
	parser.error = 0;
	ast = parse_logical_expression(&parser);
	if (parser.error)
	{
		if (tokens == NULL)
		{
			shell_context->error_flag = 1;
			cleanup_ast(ast);
			return (NULL);
		}
	}
	if (parser.current->type != TOKEN_EOF)
	{
		consume_token_type(&parser, TOKEN_EOF);
		cleanup_ast(ast);
		return (NULL);
	}
	return (ast);
}

void	cleanup_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->args)
		ft_freearr(node->args);
	if (node->file)
		free(node->file);
	cleanup_ast(node->left);
	cleanup_ast(node->right);
	free(node);
}
