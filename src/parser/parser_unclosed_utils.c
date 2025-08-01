#include "../../include/minishell.h"

int	are_quotes_unclosed(const char *s)
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
