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

/* void	expander_var(t_token *token_list)
{
	//recorrer la lista enlazada
	while (token_list != NULL)
	{
		//si token_list->expand == 1
		//recorre el token->value
			//si encuentra $
			//checkea el siguiente char
			// es valid? next
			//no es valido - sustituir con getenv
		//getenv();
		//sustituir la variable por u equivalencia
	}
} */