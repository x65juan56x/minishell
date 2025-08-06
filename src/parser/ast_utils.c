/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:40 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:22:47 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = NULL;
	node->file = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

static t_node_type	token_to_node_type(t_token_type token_type)
{
	if (token_type == TOKEN_PIPE)
		return (NODE_PIPE);
	if (token_type == TOKEN_OR)
		return (NODE_OR);
	if (token_type == TOKEN_AND)
		return (NODE_AND);
	if (token_type == TOKEN_REDIRECT_IN)
		return (NODE_REDIRECT_IN);
	if (token_type == TOKEN_REDIRECT_OUT)
		return (NODE_REDIRECT_OUT);
	if (token_type == TOKEN_REDIRECT_APPEND)
		return (NODE_REDIRECT_APPEND);
	if (token_type == TOKEN_HEREDOC)
		return (NODE_HEREDOC);
	return (NODE_COMMAND);
}

t_ast_node	*create_binary_node(t_token_type op_type, t_ast_node *left,
			t_ast_node *right)
{
	t_ast_node	*node;

	node = create_ast_node(token_to_node_type(op_type));
	if (!node)
		return (NULL);
	node->left = left;
	node->right = right;
	return (node);
}

t_ast_node	*create_redirect_node(t_token_type redirect_type, t_ast_node *cmd,
			char *file)
{
	t_ast_node	*node;

	node = create_ast_node(token_to_node_type(redirect_type));
	if (!node)
		return (NULL);
	node->left = cmd;
	node->file = ft_strdup(file);
	if (!node->file)
	{
		free(node);
		return (NULL);
	}
	return (node);
}
