/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:37 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:37:08 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*get_path_from_env(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (NULL);
	return (envp[i] + 5);
}

static char	*build_full_path(char *dir, char *cmd)
{
	char	*path_with_slash;
	char	*full_path;

	path_with_slash = ft_strjoin(dir, "/");
	if (!path_with_slash)
		return ((char *)-1);
	full_path = ft_strjoin(path_with_slash, cmd);
	free(path_with_slash);
	if (!full_path)
		return ((char *)-1);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}

static char	*search_cmd_in_dirs(char **dirs, char *cmd)
{
	char	*path;
	int		i;

	i = -1;
	while (dirs[++i])
	{
		path = build_full_path(dirs[i], cmd);
		if (path == (char *)-1)
			return (ft_freearr(dirs), NULL);
		if (path != NULL)
			return (ft_freearr(dirs), path);
	}
	ft_freearr(dirs);
	return (NULL);
}

static int	check_direct_path(char *cmd)
{
	if (access(cmd, F_OK) != 0)
		return (127);
	if (access(cmd, X_OK) != 0)
		return (126);
	return (0);
}

char	*find_command_path(char *cmd, char **envp)
{
	char	*path;
	char	**dirs;
	char	*tmp;
	int		status;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/') != NULL)
	{
		status = check_direct_path(cmd);
		if (status == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	tmp = get_path_from_env(envp);
	if (!tmp)
		return (NULL);
	dirs = ft_split(tmp, ':');
	if (!dirs)
		return (NULL);
	path = search_cmd_in_dirs(dirs, cmd);
	return (path);
}
