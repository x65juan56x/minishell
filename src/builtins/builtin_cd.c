/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:20:33 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:36:24 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	update_env_var(const char *var_name, const char *value,
							t_shell_context *shell_context)
{
	char	*new_var;
	char	*temp;
	char	*export_args[3];

	temp = ft_strjoin(var_name, "=");
	if (!temp)
		return (1);
	new_var = ft_strjoin(temp, value);
	free(temp);
	if (!new_var)
		return (1);
	export_args[0] = "export";
	export_args[1] = new_var;
	export_args[2] = NULL;
	builtin_export(export_args, shell_context);
	free(new_var);
	return (0);
}

static int	go_to_path(const char *path, t_shell_context *shell_context)
{
	char	*old_pwd_val;
	char	*old_pwd_copy;
	char	*new_pwd;

	old_pwd_val = get_env_value("PWD", shell_context);
	old_pwd_copy = NULL;
	if (old_pwd_val)
	{
		old_pwd_copy = ft_strdup(old_pwd_val);
		if (!old_pwd_copy)
			return (1);
	}
	if (chdir(path) != 0)
		return (perror("minishell: cd"), free(old_pwd_copy), 1);
	if (old_pwd_copy)
		update_env_var("OLDPWD", old_pwd_copy, shell_context);
	free(old_pwd_copy);
	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
		update_env_var("PWD", new_pwd, shell_context);
	if (new_pwd)
		free(new_pwd);
	else
		return (perror("minishell: cd: getcwd"), 1);
	return (0);
}

int	builtin_cd(char **args, t_shell_context *shell_context)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_value("HOME", shell_context);
		if (!path)
			return (ft_putendl_fd("minishell: cd: HOME not set", 2), 1);
	}
	else if (args[2])
		return (ft_putendl_fd("minishell: cd: too many arguments", 2), 1);
	else if (ft_strcmp(args[1], "") == 0)
		return (0);
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value("OLDPWD", shell_context);
		if (!path)
			return (ft_putendl_fd("minishell: cd: OLDPWD not set", 2), 1);
		ft_putendl_fd(path, STDOUT_FILENO);
	}
	else
		path = args[1];
	return (go_to_path(path, shell_context));
}
