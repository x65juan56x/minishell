#include "../../include/minishell.h"

static int	is_valid_identifier(const char *name)
{
	int	i;

	if (!name || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static int	find_env_var_index(const char *name, char **envp)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(name);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, len) == 0 && envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

static int	add_new_env_var(const char *arg, char ***envp_ptr)
{
	char	**new_envp;
	int		count;
	int		i;

	count = 0;
	while ((*envp_ptr)[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (1);
	i = -1;
	while ((*envp_ptr)[++i])
	{
		new_envp[i] = (*envp_ptr)[i];
		(*envp_ptr)[i] = NULL;
	}
	new_envp[i] = ft_strdup(arg);
	new_envp[i + 1] = NULL;
	free(*envp_ptr);
	*envp_ptr = new_envp;
	return (0);
}

int	builtin_export(char **args, char ***envp_ptr)
{
	int		i;
	char	*var_name;
	char	*eq_ptr;
	int		idx;

	if (!args[1])
		return (builtin_env(*envp_ptr));
	i = 1;
	while (args[i])
	{
		eq_ptr = ft_strchr(args[i], '=');
		if (eq_ptr)
			var_name = ft_substr(args[i], 0, eq_ptr - args[i]);
		else
			var_name = ft_strdup(args[i]);
		if (!is_valid_identifier(var_name))
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			free(var_name);
			i++;
			continue ;
		}
		idx = find_env_var_index(var_name, *envp_ptr);
		if (idx != -1 && eq_ptr)
		{
			free((*envp_ptr)[idx]);
			(*envp_ptr)[idx] = ft_strdup(args[i]);
		}
		else if (idx == -1 && eq_ptr)
			add_new_env_var(args[i], envp_ptr);
		free(var_name);
		i++;
	}
	return (0);
}

int	builtin_unset(char **args, char ***envp_ptr)
{
	int		i;
	int		idx;
	char	*var_to_remove;

	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			i++;
			continue ;
		}
		idx = find_env_var_index(args[i], *envp_ptr);
		if (idx != -1)
		{
			var_to_remove = (*envp_ptr)[idx];
			while ((*envp_ptr)[idx])
			{
				(*envp_ptr)[idx] = (*envp_ptr)[idx + 1];
				idx++;
			}
			free(var_to_remove);
		}
		i++;
	}
	return (0);
}
