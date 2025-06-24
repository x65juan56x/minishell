#include "../../include/minishell.h"

static void	skip_quoted_section(const char *s, int *i)
{
	char	quote;

	quote = s[*i];
	(*i)++;
	while (s[*i] && s[*i] != quote)
		(*i)++;
	if (s[*i] == quote)
		(*i)++;
}

static int	find_word_end(const char *s, int start)
{
	int	i;

	i = start;
	while (s[i] && !is_operator_char(s[i]) && s[i] != ' ')
	{
		if (s[i] == '\'' || s[i] == '"')
			skip_quoted_section(s, &i);
		else
			i++;
	}
	return (i);
}

static void	copy_unquoted_char(const char *s, int *k, char *result, int *j)
{
	result[*j] = s[*k];
	(*j)++;
	(*k)++;
}

static void	copy_quoted_section(const char *s, int *k, char *result, int *j)
{
	char	quote;

	quote = s[(*k)++];
	while (s[*k] && s[*k] != quote)
	{
		result[*j] = s[*k];
		(*j)++;
		(*k)++;
	}
	if (s[*k] == quote)
		(*k)++;
}

char	*process_quoted_string(const char *s, int start, int end)
{
	char	*result;
	int		j;
	int		k;

	result = malloc(end - start + 1);
	if (!result)
		return (NULL);
	j = 0;
	k = start;
	while (k < end)
	{
		if (s[k] == '\'' || s[k] == '"')
			copy_quoted_section(s, &k, result, &j);
		else
			copy_unquoted_char(s, &k, result, &j);
	}
	result[j] = '\0';
	return (result);
}

char	*extract_word_token(const char *s, int *i)
{
	int	start;
	int	end;

	start = *i;
	end = find_word_end(s, start);
	*i = end;
	return (process_quoted_string(s, start, end));
}


