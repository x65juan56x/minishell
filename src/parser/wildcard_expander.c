#include "../../include/minishell.h"

// Crea una nueva lista de tokens a partir de los nombres de fichero que coinciden.
static t_token	*create_match_tokens(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	t_token			*matches_head;
	t_token			*matches_curr;
	t_token			*new_token;

	matches_head = NULL;
	matches_curr = NULL;
	dir = opendir(".");
	if (!dir)
		return (NULL);
	entry = readdir(dir);
	while (entry != NULL)
	{
		if ((pattern[0] == '.' || entry->d_name[0] != '.')
			&& match_wildcard(entry->d_name, pattern))
		{
			new_token = create_token(TOKEN_WORD, ft_strdup(entry->d_name));
			if (!new_token)
				break ; // Error de malloc, se limpiará fuera.
			add_token(&matches_head, &matches_curr, new_token);
		}
		entry = readdir(dir);
	}
	closedir(dir);
	return (matches_head);
}

// Reemplaza un token de wildcard por una lista de tokens de coincidencias.
static void	replace_token_with_matches(t_token *prev, t_token *current,
										t_token *matches)
{
	t_token	*last_match;

	if (!matches)
		return; // No hubo coincidencias, el token original se queda.
	if (prev)
		prev->next = matches;
	last_match = matches;
	while (last_match->next)
		last_match = last_match->next;
	last_match->next = current->next;
	free(current->value);
	free(current);
}

// Función principal que recorre la lista de tokens y expande los wildcards.
// Devuelve la nueva cabeza de la lista, que puede haber cambiado.
t_token	*expand_wildcards(t_token *tokens)
{
	t_token	*current;
	t_token	*prev;
	t_token	*matches;
	t_token	*head;

	head = tokens;
	prev = NULL;
	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD && current->in_quotes == 0
			&& ft_strchr(current->value, '*'))
		{
			matches = create_match_tokens(current->value);
			if (matches)
			{
				replace_token_with_matches(prev, current, matches);
				if (prev)
					current = prev->next;
				else
				{
					head = matches; // La cabeza de la lista ha cambiado
					current = matches;
				}
				while (current && current->next)
					current = current->next;
			}
		}
		prev = current;
		current = current->next;
	}
	return (head);
}
