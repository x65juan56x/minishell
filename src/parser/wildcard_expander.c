#include "../../include/minishell.h"

void	print_token_values(t_token *head)
{
	t_token *curr = head;

	while (curr)
	{
		printf("%s\n", curr->value);
		curr = curr->next;
	}
}


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
	while ((entry = readdir(dir)) != NULL)
	{
		if ((pattern[0] == '.' || entry->d_name[0] != '.')
			&& match_wildcard(entry->d_name, pattern))
		{
			new_token = create_token(TOKEN_WORD, ft_strdup(entry->d_name));
			if (!new_token)
				break ; // Error de malloc, se limpiará fuera.
			add_token(&matches_head, &matches_curr, new_token);
		}
	}
	closedir(dir);
	return (matches_head);
}
// Crea una nueva lista de tokens a partir de los nombres de fichero que coinciden.

static void	replace_token_with_matches(t_token *prev, t_token *current,
										t_token *matches)
{
	t_token	*last_match;

	if (!matches)
		return;
	if (prev)
		prev->next = matches;
	last_match = matches;
	while (last_match->next)
		last_match = last_match->next;
	last_match->next = current->next;
	free(current->value);
	free(current);
}
// Reemplaza un token de wildcard por una lista de tokens de coincidencias.

t_token	*expand_wildcards(t_token *tokens)
{
    t_token	*current;
    t_token	*prev;
    t_token	*matches;
    t_token	*head;
    t_token	*last_match;

    head = tokens;
    prev = NULL;
    current = tokens;
    while (current)
    {
        if (current->type == TOKEN_WORD && ft_strchr(current->value, '*'))
        {
            matches = create_match_tokens(current->value);
            if (matches)
            {
                last_match = matches;
                while (last_match->next)
                    last_match = last_match->next;
				replace_token_with_matches(prev, current, matches);
                if (prev == NULL)
                    head = matches;
                prev = last_match;
                current = last_match->next;
                continue;
            }
        }
        prev = current;
        current = current->next;
    }
    return (head);
}
// Función principal que recorre la lista de tokens y expande los wildcards.
// Devuelve la nueva cabeza de la lista, que puede haber cambiado.
