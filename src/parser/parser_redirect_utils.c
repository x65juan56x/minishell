/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirect_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:59 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:18 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_HEREDOC);
}

int	is_redirect_node(t_node_type type)
{
	return (type == NODE_REDIRECT_OUT
		|| type == NODE_REDIRECT_IN
		|| type == NODE_REDIRECT_APPEND
		|| type == NODE_HEREDOC);
}
