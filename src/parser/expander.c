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

char	*do_expand(t_token *token, int *i, t_shell_context *shell_context)
{
	while (token->value[*i] != '\0')
	{
		if (token->value[*i] == '$')
			return (expand_pid(i));
		else if (token->value[*i] == '?')
			return (expand_status(i, shell_context));
		else if (token->value[*i] == '{')
			return (expand_curly(i, shell_context, token));
		else if (ft_isalpha(token->value[*i]) || token->value[*i] == '_')
			return (expand_var(i, shell_context, token));
		else
		{
			(*i)++;
			return (ft_strdup(""));
		}
	}
	return (ft_strdup(""));
}

char	*copy_non_expanded(char *value, int *i, char *var_expanded)
{
	char	char_to_string[2];
	char	*temp_string;

	char_to_string[0] = value[*i];
	char_to_string[1] = '\0';
	temp_string = ft_strjoin(var_expanded, char_to_string);
	free(var_expanded);
	var_expanded = temp_string;
	(*i)++;
	return (var_expanded);
}

static void	expand_dollar(t_shell_context *shell_context, t_token **tmp, int *i,
			char **var_expanded)
{
	char	*temp_string;
	char	*tmp_expanded;

	(*i)++;
	tmp_expanded = do_expand(*tmp, i, shell_context);
	temp_string = ft_strjoin(*var_expanded, tmp_expanded);
	free(*var_expanded);
	*var_expanded = temp_string;
	free(tmp_expanded);
}

void	update_tokens_values(t_shell_context *shell_context, t_token *tmp)
{
	char	*original_value;
	char	*var_expanded;
	int		i;

	var_expanded = ft_strdup("");
	i = 0;
	original_value = tmp->value;
	while (i < (int)ft_strlen(original_value) && original_value[i] != '\0')
	{
		if (original_value[i] == '$' && original_value[1 + i] != '\0')
			expand_dollar(shell_context, &tmp, &i, &var_expanded);
		else if (original_value[i] == '$' && original_value[1 + i] == '\0')
			var_expanded = copy_non_expanded(original_value, &i, var_expanded);
		else
			var_expanded = copy_non_expanded(original_value, &i, var_expanded);
	}
	free(original_value);
	tmp->value = var_expanded;
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
// creamos un token temporal con la linea del heredoc
// tmp_token->expand = 1; => marcar el token para que sea expandido
// llamar a la funcion de actualización de tokens
// ahora el value de este token se corresponde con el valor expandido
// extraer el valor expandido