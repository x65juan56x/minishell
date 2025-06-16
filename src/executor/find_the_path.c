/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_the_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 12:57:02 by jmondon           #+#    #+#             */
/*   Updated: 2025/06/14 00:47:40 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*paths_in_env(char **envp)
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

static char	*check_dirs(char **dirs, char *cmd)
{
	char	*path;
	int		i;

	i = -1;
	while (dirs[++i])
	{
		path = build_full_path(dirs[i], cmd);
		if (path == (char *)-1)
		{
			ft_freearr(dirs);
			return (NULL);
		}
		if (path != NULL)
		{
			ft_freearr(dirs);
			return (path);
		}
	}
	ft_freearr(dirs);
	return (NULL);
}

char	*get_path(char *cmd, char **envp)
{
	char	*path;
	char	**dirs;
	char	*tmp;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	tmp = paths_in_env(envp);
	if (!tmp)
		tmp = "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin";
	dirs = ft_split(tmp, ':');
	if (!dirs)
		return (NULL);
	path = check_dirs(dirs, cmd);
	return (path);
}
