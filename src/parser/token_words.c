#include "../../include/minishell.h"

static void	copy_inside_quotes(char *dest, int *j, const char *src, int *k)
{
	char	quote_char;

	quote_char = src[*k];
	(*k)++;
	while (src[*k] != '\0' && src[*k] != quote_char)
	{
		dest[*j] = src[*k];
		(*j)++;
		(*k)++;
	}
	if (src[*k] == quote_char)
		(*k)++;
}

static void	handle_dollar_prefix(const char *s, int *k, int *quoted)
{
	if (s[*k] == '$' && (s[*k + 1] == '\'' || s[*k + 1] == '"'))
	{
		*quoted = 2;
		(*k)++;
	}
}

static void	set_quote_type(const char *s, int *k, int *quoted)
{
	if (s[*k] == '"')
		*quoted = 1;
	else if (s[*k] == '\'')
		*quoted = 2;
}

static int	should_continue_word(const char *s, int k)
{
	return (s[k] != '\0' && !is_operator_char(s[k])
		&& s[k] != ' ' && s[k] != '\t');
}

char	*extract_word_token(const char *s, int *i, int *quoted)
{
	char	*value;
	int		k;
	int		j;

	value = malloc(ft_strlen(s) + 1);
	if (!value)
		return (NULL);
	k = *i;
	j = 0;
	while (should_continue_word(s, k))
	{
		handle_dollar_prefix(s, &k, quoted);
		if (s[k] == '\'' || s[k] == '"')
		{
			set_quote_type(s, &k, quoted);
			copy_inside_quotes(value, &j, s, &k);
		}
		else
			value[j++] = s[k++];
	}
	value[j] = '\0';
	*i = k;
	return (value);
}
