/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:01 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:29 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
