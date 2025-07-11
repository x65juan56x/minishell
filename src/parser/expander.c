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
			if (ft_isalpha(s[i]) || s[i] == '_')
			{
				while (ft_isalnum(s[i]) || s[i] == '_')
					i++;
				token->expand = 1;
			}
			//else (caracter no valido)
		}
		i++;
	}
}

char	*expand_pid(int *i)
{
	pid_t pid;
	char *pid_str;

	pid = getpid();
	pid_str = ft_itoa(pid);
	*i = *i + 2; 
	return pid_str;
}

char 	*extract_var_name(char *str, int *i)
{
	int	len;

	len = 0;
	while (str[(*i )+ len] && (ft_isalnum(str[(*i )+ len] ) || str[(*i )+ len]  == '_'))
	{
		len++;
	}	
	return(ft_substr(str, *i, len));
}

char	*do_expand(t_token *token, int *i)
{
	char	*variable;
	//int	value_len;
	char	*expanded_var;
	//int	env_var_len;

	//value_len = ft_strlen(token->value) - 1;
	while (token->value[*i] != '\0')
	{
		if (token->value[*i] == '$')
			return(expand_pid(i));
/* 		else if(token->value[*i] == '?')
		{
			//$? expands to the exit status of the most recently executed foreground pipeline.
		} */
		else if(ft_isalpha(token->value[*i]) || token->value[*i] == '_')
		{
			//variable = malloc(value_len * sizeof(char)); 
			variable = extract_var_name(token->value, i);
			*i = *i + ft_strlen(variable);
			//env_var_len = ft_strlen(getenv(variable));
			expanded_var = ft_strdup(getenv(variable));
			return (expanded_var);
		}
		else
			 return ("");
		(*i)++; /*?*/
	}
	return "";
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
void	expander_var(t_token *token_list)
{
	t_token *tmp;
	char *var_expanded;
	int i = 0;
	char *temp_string;
	char *original_value;
	//int original_len;
	char *expanded;

	tmp = token_list;

	while (tmp != NULL)
	{	//recorrer el token
		if (tmp->expand != 1)
		{
			tmp = tmp->next;
			continue;
		}
		var_expanded = ft_strdup("");
		i = 0;
		original_value = tmp->value;
		//original_len = ft_strlen(original_value); 
		while (i < (int)ft_strlen(original_value) && original_value[i] != '\0')
		{
			//si $ do expand;
			if (original_value[i] == '$')
			{
				i++;
				expanded = do_expand(tmp, &i);
				if (!expanded)
					expanded = ft_strdup("");
				temp_string = ft_strjoin(var_expanded, expanded);
				free(var_expanded);
				var_expanded = temp_string;
				free(expanded);
				//i++;
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