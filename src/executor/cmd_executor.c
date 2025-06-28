#include "../../include/minishell.h"

static void	exit_command_not_found(char **args, char *path)
{
	char	*cmd_name;

	cmd_name = ft_strdup(args[0]);
	if (path)
		free(path);
	ft_freearr(args);
	ft_cmd_not_found_exit(cmd_name);
	free(cmd_name);
}

static void	exit_execve_error(char **args, char *path)
{
	char	*cmd_name;

	cmd_name = ft_strdup(args[0]);
	free(path);
	ft_freearr(args);
	ft_execve_error_exit(cmd_name);
	free(cmd_name);
}

void	launch_command(char **args, char **envp)
{
	char	*path;

	if (!args || !args[0] || args[0][0] == '\0')
		ft_cmd_not_found_exit("");
	path = find_command_path(args[0], envp);
	if (!path)
		exit_command_not_found(args, path);
	execve(path, args, envp);
	exit_execve_error(args, path);
}
