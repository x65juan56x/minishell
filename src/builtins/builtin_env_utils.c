/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:20:56 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:20:57 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

char	*get_env_value(const char *var_name, t_shell_context *shell_context)
{
	int		i;
	size_t	len;
	t_list	*current;
	char	*content;
	char	*eq_ptr;

	if (!var_name || !shell_context || !shell_context->envp_cpy)
		return (NULL);
	len = ft_strlen(var_name);
	current = shell_context->local_vars;
	while (current)
	{
		content = (char *)current->content;
		eq_ptr = ft_strchr(content, '=');
		if (eq_ptr && (size_t)(eq_ptr - content) == len
			&& ft_strncmp(content, var_name, len) == 0)
			return (eq_ptr + 1);
		current = current->next;
	}
	i = -1;
	while (shell_context->envp_cpy && shell_context->envp_cpy[++i])
		if (ft_strncmp(shell_context->envp_cpy[i], var_name, len) == 0
			&& shell_context->envp_cpy[i][len] == '=')
			return (shell_context->envp_cpy[i] + len + 1);
	return (NULL);
}

int	is_valid_identifier(const char *name)
{
	int	i;

	if (!name || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	find_env_var_index(const char *name, t_shell_context *shell_context)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(name);
	while (shell_context->envp_cpy[i])
	{
		if (ft_strncmp(shell_context->envp_cpy[i], name, len) == 0
			&& shell_context->envp_cpy[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

int	add_new_env_var(const char *arg, t_shell_context *shell_context)
{
	int		count;
	char	**new_envp;
	int		i;

	count = 0;
	while (shell_context->envp_cpy[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (1);
	i = -1;
	while (++i < count)
	{
		new_envp[i] = shell_context->envp_cpy[i];
	}
	new_envp[i] = ft_strdup(arg);
	if (!new_envp[i])
		return (free(new_envp), 1);
	new_envp[i + 1] = NULL;
	free(shell_context->envp_cpy);
	shell_context->envp_cpy = new_envp;
	return (0);
}

void	remove_local_var(const char *name, t_list **local_vars)
{
	t_list	*prev;
	t_list	*current;
	size_t	name_len;
	char	*content;

	prev = NULL;
	current = *local_vars;
	name_len = ft_strlen(name);
	while (current)
	{
		content = (char *)current->content;
		if (ft_strncmp(content, name, name_len) == 0
			&& (content[name_len] == '=' || content[name_len] == '\0'))
		{
			if (prev)
				prev->next = current->next;
			else
				*local_vars = current->next;
			ft_lstdelone(current, free);
			return ;
		}
		prev = current;
		current = current->next;
	}
}
