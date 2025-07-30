#include "../../include/minishell.h"

int	builtin_echo(char **args)
{
	int	newline;
	int	i;

	newline = 1;
	i = 1;
	while (args[i] && (ft_strcmp(args[i], "-n") == 0))
	{
		newline = 0;
		i++;
	}
	while(args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (perror("minishell: pwd"), 1);
	ft_putendl_fd(cwd, STDOUT_FILENO);
	free(cwd);
	return (0);
}

int	builtin_env(char **envp, char **args)
{
	int	i;

	if (!envp)
	{
		ft_putstr_fd("minishell: env: invalid environment\n", STDERR_FILENO);
		return (1);
	}
	if (args[1])
	{
		ft_putstr_fd("minishell: env: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	i = 0;
	while (envp[i])
	{
		if (ft_putstrplus_fd(STDOUT_FILENO, envp[i]) == -1)
			return (perror("minishell: env"), 1);
		if (ft_putcharplus_fd(STDOUT_FILENO, '\n') == -1)
			return (perror("minishell: env"), 1);
		i++;
	}
	return (0);
}

int	get_exit_status_from_args(t_token *args_token)
{
	if (!args_token || args_token->type == TOKEN_EOF)
		return (0);
	if (!ft_isnumstr(args_token->value))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args_token->value, STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		return (2);
	}
	if (args_token->next && args_token->next->type != TOKEN_EOF)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	return ((unsigned char)ft_atoi(args_token->value));
}
// Esta función calcula el código de salida para 'exit' sin terminar el proceso.
// Se usa solo en el bucle principal para una salida limpia.
// 1. Primero, validar si el primer argumento es numérico.
// 2. Si es numérico, entonces comprobar si hay demasiados argumentos.
// 3. Si todo está bien, devolver el código de salida correcto.

int	builtin_exit(char **args)
{
	int	exit_code;

	if (!args[1])
		exit(0);
	if (!ft_isnumstr(args[1]))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		exit(2);
	}
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	exit_code = ft_atoi(args[1]);
	exit((unsigned char)exit_code);
}
// 1. Primero, validar si el primer argumento es numérico.
// 2. Si es numérico, entonces comprobar si hay demasiados argumentos.
// 3. Si todo está bien, salir con el código correcto.
