#include "../../include/minishell.h"

static char	**copy_envp(char **envp)
{
	char	**new_envp;
	int		i;
	int		count;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = -1;
	while (++i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (ft_freearr(new_envp), NULL);
	}
	new_envp[i] = NULL;
	return (new_envp);
}

char	**init_shell_environment(char **envp, t_shell_context *shell_context)
{
	char	**envp_copy;

	envp_copy = copy_envp(envp);
	if (!envp_copy)
	{
		ft_putstr_fd("minishell: failed to initialize environment\n",
			STDERR_FILENO);
		cleanup_shell_context(shell_context);
		exit(1);
	}
	return (envp_copy);
}
