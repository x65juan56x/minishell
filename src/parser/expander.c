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
				while (ft_isalnum(s[i]) || s[i] == '_')
					i++;
				token->expand = 1;
			}
		}
		i++;
	}
}

char	*expand_pid(int *i)
{
	pid_t	pid;
	char	*pid_str;

	pid = getpid();
	pid_str = ft_itoa(pid);
	*i = *i + 2; 
	return (pid_str);
}

char 	*expand_status(int *i, t_shell_context *shell_context)
{
	int		status;
	char	*status_str;
	int		len;

	status = shell_context->exit_status;
	status_str = ft_itoa(status);
	len = ft_strlen(status_str);
	*i = *i + len;
	return(status_str);
}

char 	*extract_var_name(char *str, int *i)
{
	int	len;

	len = 0;
	while (str[(*i) + len] && (ft_isalnum(str[(*i )+ len] ) || str[(*i )+ len]  == '_'))
		len++;
	return(ft_substr(str, *i, len));
}

char	*expand_var(int *i, t_shell_context *shell_context, t_token *token)
{
	char	*env_value;
	char	*variable;

	variable = extract_var_name(token->value, i);
	*i = *i + ft_strlen(variable);
	env_value = get_env_value(variable, shell_context);
	free(variable);
	if (env_value)
		return(ft_strdup(env_value));
	return (ft_strdup(""));
}

char	*expand_curly(int *i, t_shell_context *shell_context, t_token *token)
{
	char	*env_value;

	(*i)++;
	env_value = expand_var(i, shell_context, token);
	if (token->value[*i] != '}')
		return (ft_strdup(""));
	else
		(*i)++;
	if (env_value)
		return(ft_strdup(env_value));
	return (ft_strdup(""));
}

char	*do_expand(t_token *token, int *i, t_shell_context *shell_context)
{
	while (token->value[*i] != '\0')
	{
		if (token->value[*i] == '$')
			return(expand_pid(i));
 		else if(token->value[*i] == '?')
			return(expand_status(i, shell_context));
		else if (token->value[*i] == '{')
			return(expand_curly(i, shell_context, token));
		else if(ft_isalpha(token->value[*i]) || token->value[*i] == '_')
			return(expand_var(i, shell_context, token));
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
void	expander_var(t_token *token_list, t_shell_context *shell_context)
{
	t_token *tmp;
	char *var_expanded;
	int i;
	char *temp_string;
	char *original_value;
	char *tmp_expanded;

	tmp = token_list;
	while (tmp != NULL)
	{
		if (tmp->expand != 1)
		{
			tmp = tmp->next;
			continue ;
		}
		var_expanded = ft_strdup("");
		i = 0;
		original_value = tmp->value;
		while (i < (int)ft_strlen(original_value) && original_value[i] != '\0')
		{
			if (original_value[i] == '$')
			{
				i++;
				tmp_expanded = do_expand(tmp, &i, shell_context);
				temp_string = ft_strjoin(var_expanded, tmp_expanded);
				free(var_expanded);
				var_expanded = temp_string;
				free(tmp_expanded);
			}
			else 
				var_expanded = copy_non_expanded(original_value, &i, var_expanded);
		}
		free(tmp->value);
		tmp->value = var_expanded;
		tmp = tmp->next;
	}
	return ;
}

//PROBLEMA DE SEGMENTATION FAULT CUANDO ES UN ENV_VAR INCORRECTO