#include "../../include/minishell.h"

static int	match_file(const char *pattern, const char *filename)
{
	if ((pattern[0] == '.' || filename[0] != '.')
		&& match_wildcard(filename, pattern))
		return (1);
	else
		return (0);
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
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (match_file(pattern, entry->d_name))
		{
			new_token = create_token(TOKEN_WORD, ft_strdup(entry->d_name));
			if (!new_token)
				break ;
			add_token(&matches_head, &matches_curr, new_token);
		}
		entry = readdir(dir);
	}
	closedir(dir);
	return (matches_head);
}

static t_token	*replace_token_with_matches(t_token *prev, t_token *current,
										t_token *matches)
{
	t_token	*last_match;

	if (!matches)
		return (NULL);
	if (prev)
		prev->next = matches;
	last_match = matches;
	while (last_match->next)
		last_match = last_match->next;
	last_match->next = current->next;
	free(current->value);
	free(current);
	return (last_match);
}

static void	process_wildcard_token(t_token **current_ptr, t_token **prev_ptr,
									t_token **head_ptr)
{
	t_token	*matches;
	t_token	*current;
	t_token	*prev;

	current = *current_ptr;
	prev = *prev_ptr;
	matches = create_match_tokens(current->value);
	if (matches)
	{
		prev = replace_token_with_matches(prev, current, matches);
		if (!(*head_ptr) || *head_ptr == current)
			*head_ptr = matches;
		*current_ptr = prev;
	}
	*prev_ptr = *current_ptr;
	*current_ptr = (*current_ptr)->next;
}

t_token	*expand_wildcards(t_token *tokens)
{
	t_token	*current;
	t_token	*prev;
	t_token	*head;

	head = tokens;
	prev = NULL;
	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD && ft_strchr(current->value, '*'))
			process_wildcard_token(&current, &prev, &head);
		else
		{
			prev = current;
			current = current->next;
		}
	}
	return (head);
}
