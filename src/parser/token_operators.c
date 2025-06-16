#include "../../include/minishell.h"

int	is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&' 
		|| c == '(' || c == ')');
}

static t_token_type	get_pipe_operator(const char *s, int *advance)
{
	if (s[1] == '|')
	{
		*advance = 2;
		return (TOKEN_OR);
	}
	*advance = 1;
	return (TOKEN_PIPE);
}

static t_token_type	get_redirect_operator(const char *s, int *advance)
{
	if (s[0] == '<')
	{
		if (s[1] == '<')
		{
			*advance = 2;
			return (TOKEN_HEREDOC);
		}
		*advance = 1;
		return (TOKEN_REDIRECT_IN);
	}
	if (s[1] == '>')
	{
		*advance = 2;
		return (TOKEN_REDIRECT_APPEND);
	}
	*advance = 1;
	return (TOKEN_REDIRECT_OUT);
}

t_token_type	get_operator_type(const char *s, int *advance)
{
	if (s[0] == '|')
		return (get_pipe_operator(s, advance));
	if (s[0] == '<' || s[0] == '>')
		return (get_redirect_operator(s, advance));
	if (s[0] == '&' && s[1] == '&')
	{
		*advance = 2;
		return (TOKEN_AND);
	}
	if (s[0] == '(')
	{
		*advance = 1;
		return (TOKEN_LPAREN);
	}
	if (s[0] == ')')
	{
		*advance = 1;
		return (TOKEN_RPAREN);
	}
	*advance = 1;
	return (TOKEN_WORD);
}
