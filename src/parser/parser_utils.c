/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:48 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:20 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*consume_token_type(t_parser *parser, t_token_type type)
{
	t_token	*temp;

	if (!parser->current || parser->current->type != type)
	{
		if (!parser->error)
		{
			ft_putstr_fd("minishell: syntax error near", STDERR_FILENO);
			ft_putstr_fd(" unexpected token `", STDERR_FILENO);
			if (parser->current && parser->current->type != TOKEN_EOF)
				ft_putstr_fd(parser->current->value, STDERR_FILENO);
			else
				ft_putstr_fd("newline", STDERR_FILENO);
			ft_putstr_fd("'\n", STDERR_FILENO);
		}
		parser->error = 1;
		return (NULL);
	}
	temp = parser->current;
	parser->current = temp->next;
	return (temp);
}
