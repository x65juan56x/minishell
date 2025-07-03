#include "../../include/minishell.h"

static char	*get_env_var(char **envp, char *var)
{
	int	i;
	int	len;

	len = ft_strlen(var);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

int	builtin_cd(char **args, char **envp)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_var(envp, "HOME");
		if (!path)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (1);
		}
	}
	else
		path = args[1];
	if (chdir(path) != 0)
		return (perror("minishell: cd"), 1);
	return (0);
}
