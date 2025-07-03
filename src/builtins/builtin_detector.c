#include "../../include/minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

int	execute_builtin(char **args, char ***envp_ptr)
{
	if (ft_strcmp(args[0], "echo") == 0)
		return (builtin_echo(args));
	if (ft_strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, *envp_ptr));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (builtin_pwd());
	// if (ft_strcmp(args[0], "export") == 0)
	// 	return (builtin_export(args, envp_ptr));
	// if (ft_strcmp(args[0], "unset") == 0)
	// 	return (builtin_unset(args, envp_ptr));
	if (ft_strcmp(args[0], "env") == 0)
		return (builtin_env(*envp_ptr));
	if (ft_strcmp(args[0], "exit") == 0)
		return (builtin_exit(args));
	return (1);
}
