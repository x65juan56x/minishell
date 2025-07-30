#include "../../include/minishell.h"

int	builtin_echo(char **args)
{
	int	newline;
	int	i;

	newline = 1;
	i = 1;
	while (args[i] && (ft_strcmp(args[i], "-n") == 0))
	{
		newline = 0; // No imprimir \n al final
		i++; // Empezar desde args[2]
	}
	while(args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO); // Espacio entre args
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0); // Obtener directorio actual
	if (!cwd)
	{
		perror("minishell: pwd");
		return (1);
	}
	ft_putendl_fd(cwd, STDOUT_FILENO); // liberar memoria alocada por getcwd()
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
		if (ft_putstrplus_fd(STDOUT_FILENO, envp[i]) == -1) // Escribir la variable de entorno
			return (perror("minishell: env"), 1);
		if (ft_putcharplus_fd(STDOUT_FILENO, '\n') == -1) // Escribir el newline
			return (perror("minishell: env"), 1);
		i++;
	}
	return (0);
}

int	builtin_exit(char **args)
{
	int	exit_code;

//	ft_putendl_fd("exit", STDOUT_FILENO); //DESCOMENTAR!!!
	if (!args[1])
		return (-2); // Código especial para exit sin argumentos
	if (!ft_isnumstr(args[1])) // Non-numeric argument: print error and exit with 255.
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		return (-4);
	}
	if (args[2]) // Numeric argument followed by another: print error and *do not exit*.
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	exit_code = ft_atoi(args[1]); // Valid numeric argument, and it's the only one.
	// The exit code is an 8-bit unsigned value (0-255).
	// Casting to unsigned char correctly wraps the value.
	return (-2 - (unsigned char)exit_code);
}
