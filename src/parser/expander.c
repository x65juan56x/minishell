#include "../../include/minishell.h"

void	is_expand_needed(char *s, int quoted, t_token *token)
{
	int	i;

	if (quoted == 2)
		return ;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '$')
		{
			i++;
			if (s[i] != '\0' && (s[i] == '?' || s[i] == '$'))
			{
				token->expand = 1;
				return ;
			}
			if (ft_isalpha(s[i]) || s[i] == '_' || s[i] == '{')
			{
				while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
					i++;
				token->expand = 1;
			}
		}
		else if (s[i])
			i++;
	}
}

void	expander_var(t_token *token_list, t_shell_context *shell_context)
{
	t_token	*tmp;

	tmp = token_list;
	while (tmp != NULL)
	{
		if (tmp->expand != 1)
		{
			tmp = tmp->next;
			continue ;
		}
		update_tokens_values(shell_context, tmp);
		tmp = tmp->next;
	}
	return ;
}

char	*expander_line_content(char *line, t_shell_context *shell_context)
{
	t_token	*tmp_token;
	char	*expanded_value;

	tmp_token = create_token(TOKEN_WORD, ft_strdup(line));
	if (!tmp_token)
		return (NULL);
	tmp_token->expand = 1;
	update_tokens_values(shell_context, tmp_token);
	expanded_value = tmp_token->value;
	tmp_token->value = NULL;
	cleanup_tokens(tmp_token);
	return (expanded_value);
}
