#include "../../include/minishell.h"

static int	printnget_command_error(char *cmd)
{
	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK) != 0)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd, STDERR_FILENO);
			ft_putendl_fd(": No such file or directory", STDERR_FILENO);
			return (127);
		}
		else if (access(cmd, X_OK) != 0)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd, STDERR_FILENO);
			ft_putendl_fd(": Permission denied", STDERR_FILENO);
			return (126);
		}
	}
	else
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putendl_fd(": Command not found", STDERR_FILENO);
	}
	return (127);
}

static int	no_path_to_launch(char **cmd_name_copy,
				t_shell_context *shell_context)
{
	int		error_code;

	error_code = printnget_command_error(*cmd_name_copy);
	free(*cmd_name_copy);
	cleanup_child_process_deep(shell_context);
	return (error_code);
}

void	launch_command(char **args, char **envp, t_shell_context *shell_context)
{
	char	*path;
	char	*cmd_name_copy;

	if (!args || !args[0] || args[0][0] == '\0')
	{
		cleanup_child_process_deep(shell_context);
		exit(127);
	}
	cmd_name_copy = ft_strdup(args[0]);
	if (!cmd_name_copy)
	{
		cleanup_child_process_deep(shell_context);
		exit(127);
	}
	path = find_command_path(args[0], envp);
	if (!path)
		exit(no_path_to_launch(&cmd_name_copy, shell_context));
	free(cmd_name_copy);
	execve(path, args, envp);
	perror("minishell: execve");
	free(path);
	cleanup_child_process_deep(shell_context);
	exit(126);
}
