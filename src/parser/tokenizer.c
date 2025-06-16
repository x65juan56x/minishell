#include "../../include/minishell.h"

static void skip_spaces(const char *input, int *i)
{
	while (input[*i] && input[*i] == ' ')
		(*i)++;
}

t_token			*create_eof_token(void)
{
	return (create_token(TOKEN_EOF, NULL));
}

static t_token  *process_operator(const char *input, int *i)
{
	int				advance;
	t_token_type	type;
	char			*value;

	advance = 0;
	type = get_operator_type(&input[*i], &advance);
	value = ft_substr(input, *i, advance);
	if (!value)
		return(NULL);
	*i += advance;
	return (create_token(type, value));
}

static t_token	*process_word(const char *input, int *i)
{
	char	*value;

	value = extract_word_token(input, i);
	if (!value)
		return(NULL);
	return (create_token(TOKEN_WORD, value));
}

t_token	*tokenize(const char *input)
{
	t_token	*head;
	t_token	*current;
	t_token	*new_token;
	int		i;

	if (!input)
		return(NULL);
	head = NULL;
	current = NULL;
	i = 0;
	while(input[i])
	{
		skip_spaces(input, &i);
		if (!input[i])
			break ;
		if (is_operator_char(input[i]))
			new_token = process_operator(input, &i);
		else
			new_token = process_word(input, &i);
		if (!new_token)
			return (cleanup_tokens(head), NULL);
		add_token(&head, &current, new_token);
	}
	new_token = create_eof_token();
	if (!new_token)
		return (cleanup_tokens(head), NULL);
	add_token(&head, &current, new_token);
	return (head);
}
