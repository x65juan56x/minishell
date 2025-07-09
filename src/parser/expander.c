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

char	*expand_pid()
{
	char *pid_str;
	pid_t	pid;

	pid = getpid();
	pid_str = ft_itoa(pid);
	return (pid_str);
}
char	*do_expand(t_token *token, int *i)
{
	//int length = 0;
	char *pid;
	printf("entra\n");
	printf("i: %i\n", *i);
	while (token->value[*i] != '\0')
	{
		(*i)++;
		printf("i: %i\n", *i);
		if (token->value[*i] == '$')
		{
			pid = expand_pid();
			//token->value = pid;
			printf("token->value: %s\n", token->value);
			return (pid);
		}
/* 		else if (token->value[i] == '?')
		{
			//devolver ultimo comando;
		}
		if (ft_isalpha(s[i]) || s[i] == '_')
		{
			while (ft_isalnum(s[i]) || s[i] == '_')
				i++;
			token->expand = 1;
		}
		//else (caracter no valido) */
		(*i)++;
	}
	return("NULO\n");
}

void print_list(t_token *head)
{
    t_token *current = head;
    int token_count = 0;

    printf("--- Lista de Tokens ---\n");
    if (current == NULL)
    {
        printf("La lista está vacía.\n");
        printf("-----------------------\n");
        return;
    }

    while (current != NULL)
    {
        printf("Token %d:\n", token_count);
        printf("  Value: \"%s\"\n", current->value ? current->value : "(NULL)");
        printf("  Expand: %d\n", current->expand);
        current = current->next;
        token_count++;
    }
    printf("-----------------------\n");
}
char*	expander_var(t_token *token_list)
{
	t_token *tmp;
	char *var_expanded;
	int i = 0;
	char *temp_string;
	char char_to_string[2];
	//printf("EXPANDER_VAR\n");
	//printf("EXPANDER_VAR\n");
	tmp = token_list;
	var_expanded = ft_strdup("");
	//printf("EXPANDER_VAR\n");
	//recorrer la lista enlazada
	while (tmp != NULL)
	{	//recorrer el token
		if (token_list->expand != 1)
			tmp = tmp->next;
		i = 0;
		while (tmp->value[i] != '\0')
		{
			printf("EXPANDER_VAR token->value: %s\n", tmp->value);
			//si empieza por $$ do expand;
			if (tmp->value[i] == '$')
			{
				var_expanded = do_expand(tmp, &i);
				printf("i: %i\n", i);
				tmp->value = var_expanded;
				return var_expanded;
			}
				//else copia tal cual
			else
			{
				char_to_string[0] = tmp->value[i];
				char_to_string[1] = '\0';
				temp_string = ft_strjoin(var_expanded, char_to_string);
				free(var_expanded);
				var_expanded = temp_string;
				// DUDA free(temp_string);
				i++;
			}
		}
		tmp = tmp->next;
	}
	
	return var_expanded;
}