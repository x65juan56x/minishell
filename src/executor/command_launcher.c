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
/*
 * Propósito: Resolver y ejecutar un comando externo.
 * Mecanismo:
 *   1. Verifica que `args` y `args[0]` sean válidos; si no, sale 127.
 *   2. Llama a `find_command_path` para obtener la ruta absoluta.
 *   3. Si no existe, invoca `exit_command_not_found`.
 *   4. Llama a `execve(path, args, envp)`.
 *   5. Si `execve` retorna, invoca `exit_execve_error`.
 * Llamado por: Procesos hijos que deben ejecutar un `NODE_COMMAND`.
 * Llama a:
 *   - `find_command_path`
 *   - `execve`
 *   - `exit_command_not_found`
 *   - `exit_execve_error`
 */
