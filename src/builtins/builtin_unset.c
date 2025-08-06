/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:24 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:36:33 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	remove_env_var(int idx, t_shell_context *shell_context)
{
	char	*var_to_remove;

	var_to_remove = (shell_context->envp_cpy)[idx];
	while ((shell_context->envp_cpy)[idx])
	{
		(shell_context->envp_cpy)[idx] = (shell_context->envp_cpy)[idx + 1];
		idx++;
	}
	free(var_to_remove);
}

int	builtin_unset(char **args, t_shell_context *shell_context)
{
	int		i;
	int		idx;
	int		status;

	i = 1;
	status = 0;
	while (args[i])
	{
		idx = find_env_var_index(args[i], shell_context);
		if (idx != -1)
			remove_env_var(idx, shell_context);
		remove_local_var(args[i], &shell_context->local_vars);
		i++;
	}
	return (status);
}
