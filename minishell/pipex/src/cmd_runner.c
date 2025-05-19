/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:31:24 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/14 13:21:55 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static void	split_fails(char *cmd_to_report, char **splited_cmd)
{
	if (splited_cmd)
		ft_freearr(splited_cmd);
	ft_cmd_not_found_exit(cmd_to_report);
	free(cmd_to_report);
}

static void	get_path_fails(char *cmd_to_report, char **splited_cmd)
{
	cmd_to_report = ft_strdup(splited_cmd[0]);
	ft_freearr(splited_cmd);
	ft_cmd_not_found_exit(cmd_to_report);
	free(cmd_to_report);
}

void	run_cmd(char *cmd_str, char **envp)
{
	char	**splited_cmd;
	char	*path;
	char	*cmd_to_report;

	if (!cmd_str || cmd_str[0] == '\0')
		ft_cmd_not_found_exit("");
	splited_cmd = ft_split_args(cmd_str);
	if (!splited_cmd || !splited_cmd[0])
	{
		cmd_to_report = ft_strdup(cmd_str);
		split_fails(cmd_to_report, splited_cmd);
	}
	path = get_path(splited_cmd[0], envp);
	if (!path)
		get_path_fails(cmd_to_report, splited_cmd);
	execve(path, splited_cmd, envp);
	cmd_to_report = ft_strdup(splited_cmd[0]);
	free(path);
	ft_freearr(splited_cmd);
	ft_permission_denied_exit(cmd_to_report);
	free(cmd_to_report);
}
