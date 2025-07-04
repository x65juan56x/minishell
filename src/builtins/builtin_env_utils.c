#include "../../include/minishell.h"

static int	count_env_vars(char **envp)
{
	int	count;

	count = 0;
	while (envp[count])
		count++;
	return (count);
}

static char	**realloc_envp(char **envp, int new_size)
{
	char	**new_envp;
	int		i;

	new_envp = malloc(sizeof(char *) * (new_size + 1));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (envp[i] && i < new_size)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (ft_freearr(new_envp), NULL);
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
}

int	builtin_export(char **args, char ***envp_ptr)
{
	char	**new_envp;
	int		count;
	int		i;

	if (!args[1])
		return (builtin_env(*envp_ptr), 0);
	count = count_env_vars(*envp_ptr);
	new_envp = realloc_envp(*envp_ptr, count + 1);
	if (!new_envp)
		return (1);
	i = 0;
	while (new_envp[i])
		i++;
	new_envp[i] = ft_strdup(args[1]);
	new_envp[i + 1] = NULL;
	ft_freearr(*envp_ptr);
	*envp_ptr = new_envp;
	return (0);
}

int	builtin_unset(char **args, char ***envp_ptr)
{
	char	**new_envp;
	int		count;
	int		i;
	int		j;

	if (!args[1])
		return (0);
	count = count_env_vars(*envp_ptr);
	new_envp = malloc(sizeof(char *) * count);
	if (!new_envp)
		return (1);
	i = -1;
	j = 0;
	while ((*envp_ptr)[++i])
		if (ft_strncmp((*envp_ptr)[i], args[1], ft_strlen(args[1])) != 0
			|| (*envp_ptr)[i][ft_strlen(args[1])] != '=')
			new_envp[j++] = ft_strdup((*envp_ptr)[i]);
	new_envp[j] = NULL;
	ft_freearr(*envp_ptr);
	*envp_ptr = new_envp;
	return (0);
}
