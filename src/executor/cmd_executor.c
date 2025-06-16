#include "../../include/minishell.h"

static void	handle_cmd_error(char **args, char *path)
{
	char	*cmd_name;

	cmd_name = ft_strdup(args[0]);
	if (path)
		free(path);
	ft_freearr(args);
	ft_cmd_not_found_exit(cmd_name);
	free(cmd_name);
}

static void	handle_execve_error(char **args, char *path)
{
	char	*cmd_name;

	cmd_name = ft_strdup(args[0]);
	free(path);
	ft_freearr(args);
	ft_permission_denied_exit(cmd_name);
	free(cmd_name);
}

void	run_cmd_from_args(char **args, char **envp)
{
	char	*path;

	if (!args || !args[0] || args[0][0] == '\0')
		ft_cmd_not_found_exit("");
	path = get_path(args[0], envp);
	if (!path)
		handle_cmd_error(args, path);
	execve(path, args, envp);
	handle_execve_error(args, path);
}
