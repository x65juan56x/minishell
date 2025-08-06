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

int	handle_word_token(t_token **token, char **args, int *i)
{
	t_token	*temp;

	temp = *token;
	args[*i] = ft_strdup(temp->value);
	if (!args[*i])
		return (-1);
	(*i)++;
	*token = temp->next;
	return (0);
}

void	skip_redirect_token(t_token **token)
{
	t_token	*temp_token;

	temp_token = *token;
	temp_token = temp_token->next;
	if (temp_token && temp_token->type == TOKEN_WORD)
		temp_token = temp_token->next;
	*token = temp_token;
}

int	extract_args(char **args, int max, t_token **token)
{
	int		i;
	t_token	*temp_token;

	i = 0;
	temp_token = *token;
	while (i < max && temp_token && temp_token->type == TOKEN_WORD)
		if (handle_word_token(&temp_token, args, &i) < 0)
			return (-1);
	*token = temp_token;
	return (i);
}

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
