/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:25 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:25 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static t_token	*process_operator(const char *input, int *i)
{
	int				advance;
	t_token_type	type;
	char			*value;

	advance = 0;
	type = get_operator_type(&input[*i], &advance);
	value = ft_substr(input, *i, advance);
	if (!value)
		return (NULL);
	*i += advance;
	return (create_token(type, value));
}

static t_token	*process_word(const char *input, int *i)
{
	char	*value;
	t_token	*token;
	int		quoted;

	quoted = 0;
	value = extract_word_token(input, i, &quoted);
	if (!value)
		return (NULL);
	token = create_token(TOKEN_WORD, value);
	is_expand_needed (value, quoted, token);
	return (token);
}

static int	generate_token_list(const char *input, t_token **head,
			t_token **current)
{
	int		i;
	t_token	*new_token;

	i = 0;
	while (input[i])
	{
		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
			i++;
		if (!input[i])
			break ;
		if (is_operator_char(input[i]))
			new_token = process_operator(input, &i);
		else
			new_token = process_word(input, &i);
		if (!new_token)
			return (1);
		add_token(head, current, new_token);
	}
	return (0);
}

t_token	*tokenize(const char *input)
{
	t_token	*head;
	t_token	*current;
	t_token	*new_token;

	if (!input)
		return (NULL);
	head = NULL;
	current = NULL;
	if (generate_token_list(input, &head, &current) != 0)
		return (cleanup_tokens(head), NULL);
	new_token = create_token(TOKEN_EOF, NULL);
	if (!new_token)
		return (cleanup_tokens(head), NULL);
	add_token(&head, &current, new_token);
	return (head);
}
