/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_implementations.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:17 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:21:18 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

static int	is_n_flag(const char *s)
{
	int	i;

	if (!s || s[0] != '-')
		return (0);
	i = 1;
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0' && i > 1);
}

int	builtin_echo(char **args)
{
	int	newline;
	int	i;

	newline = 1;
	i = 1;
	while (args[i] && is_n_flag(args[i]))
	{
		newline = 0;
		i++;
	}
	while (args[i])
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

int	builtin_exit(char **args, t_shell_context *shell_context)
{
	int	exit_code;

	if (!args[1])
	{
		shell_context->should_exit = 1;
		return (shell_context->exit_status = 0, 0);
	}
	if (!ft_isnumstr(args[1]))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		shell_context->should_exit = 1;
		return (shell_context->exit_status = 2, 2);
	}
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	exit_code = ft_atoi(args[1]);
	shell_context->should_exit = 1;
	shell_context->exit_status = (unsigned char)exit_code;
	return ((unsigned char)exit_code);
}
// 1. Primero, validar si el primer argumento es numérico.
// 2. Si es numérico, entonces comprobar si hay demasiados argumentos.
// 3. Si todo está bien, salir con el código correcto.
