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

void	launch_command(char **args, char **envp)
{
	char	*path;

	if (!args || !args[0] || args[0][0] == '\0')
		exit(127);
	path = find_command_path(args[0], envp);
	if (!path)
		exit(printnget_command_error(args[0]));
	execve(path, args, envp);
	perror("minishell: execve");
	free(path);
	exit(126);
}
