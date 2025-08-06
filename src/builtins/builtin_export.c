/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:12 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:21:13 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

static void	handle_export_with_value(char *arg, char *name,
			t_shell_context *shell_context)
{
	int	i;

	i = find_env_var_index(name, shell_context);
	if (i != -1)
	{
		free((shell_context->envp_cpy)[i]);
		(shell_context->envp_cpy)[i] = ft_strdup(arg);
	}
	else
		add_new_env_var(arg, shell_context);
	remove_local_var(name, &shell_context->local_vars);
}

static void	handle_export_no_value(char *name, t_shell_context *shell_context)
{
	t_list	*current;
	char	*var_content;
	size_t	name_len;

	current = shell_context->local_vars;
	name_len = ft_strlen(name);
	while (current)
	{
		var_content = (char *)current->content;
		if (ft_strncmp(var_content, name, name_len) == 0
			&& (var_content[name_len] == '=' || var_content[name_len] == '\0'))
		{
			add_new_env_var(var_content, shell_context);
			remove_local_var(name, &shell_context->local_vars);
			break ;
		}
		current = current->next;
	}
}

static int	process_export_arg(char *arg, t_shell_context *shell_context)
{
	char	*var_name;
	char	*eq_ptr;

	eq_ptr = ft_strchr(arg, '=');
	if (eq_ptr)
		var_name = ft_substr(arg, 0, eq_ptr - arg);
	else
		var_name = ft_strdup(arg);
	if (!is_valid_identifier(var_name))
	{
		ft_putstr_fd("minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
		free(var_name);
		return (1);
	}
	if (eq_ptr)
		handle_export_with_value(arg, var_name, shell_context);
	else
		handle_export_no_value(var_name, shell_context);
	free(var_name);
	return (0);
}

int	builtin_export(char **args, t_shell_context *shell_context)
{
	int	i;
	int	exit_status;

	exit_status = 0;
	if (!args[1])
		return (sort_and_print_export(shell_context->envp_cpy), 0);
	i = 0;
	while (args[++i])
		if (process_export_arg(args[i], shell_context) != 0)
			exit_status = 1;
	return (exit_status);
}
