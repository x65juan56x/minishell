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
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			status = 1;
		}
		else
		{
			idx = find_env_var_index(args[i], shell_context);
			if (idx != -1)
				remove_env_var(idx, shell_context);
			remove_local_var(args[i], &shell_context->local_vars);
		}
		i++;
	}
	return (status);
}
