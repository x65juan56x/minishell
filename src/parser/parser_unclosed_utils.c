#include "../../include/minishell.h"

static char	get_quote_state(const char *s)
{
	char	in_quote;

	in_quote = 0;
	while (*s)
	{
		if (*s == '\'' && in_quote == 0)
			in_quote = '\'';
		else if (*s == '"' && in_quote == 0)
			in_quote = '"';
		else if (*s == in_quote)
			in_quote = 0;
		s++;
	}
	return (in_quote);
}

static int	find_last_non_space(const char *s)
{
	int	i;

	if (!s)
		return (-1);
	i = ft_strlen(s) - 1;
	while (i >= 0 && (s[i] == ' ' || s[i] == '\t'))
		i--;
	return (i);
}

int	are_quotes_unclosed(const char *s)
{
	return (get_quote_state(s) != 0);
}

int	are_parentheses_unclosed(const char *s)
{
	int		i;
	int		paren_level;
	char	in_quote;

	i = 0;
	paren_level = 0;
	in_quote = 0;
	while (s[i])
	{
		if (s[i] == '\'' && in_quote == 0)
			in_quote = '\'';
		else if (s[i] == '"' && in_quote == 0)
			in_quote = '"';
		else if (s[i] == in_quote)
			in_quote = 0;
		else if (s[i] == '(' && in_quote == 0)
			paren_level++;
		else if (s[i] == ')' && in_quote == 0)
			paren_level--;
		i++;
	}
	return (paren_level > 0);
}

int	has_pending_logical_operator(const char *input)
{
	int	i;

	if (!input || get_quote_state(input) != 0)
		return (0);
	i = find_last_non_space(input);
	if (i < 1)
		return (0);
	if (i >= 1 && input[i] == '&' && input[i - 1] == '&')
		return (1);
	if (i >= 1 && input[i] == '|' && input[i - 1] == '|')
		return (1);
	if (i >= 0 && input[i] == '|' && (i == 0 || input[i - 1] != '|'))
		return (1);
	return (0);
}
