#include "../../include/minishell.h"

// Función auxiliar para obtener el valor de una variable de nuestro entorno.
// No usamos getenv() porque debemos leer de nuestra copia privada.
char	*get_env_value(const char *var_name, t_shell_context *shell_context)
{
	int		i;
	size_t	len;

	if (!var_name || !shell_context || !shell_context->envp_cpy)
		return (NULL);
	i = 0;
	len = ft_strlen(var_name);
	while (shell_context->envp_cpy[i])
	{
		if (ft_strncmp(shell_context->envp_cpy[i], var_name, len) == 0 && shell_context->envp_cpy[i][len] == '=')
			return (shell_context->envp_cpy[i] + len + 1);
		i++;
	}
	return (NULL);
}

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

static int	find_env_var_index(const char *name, t_shell_context *shell_context)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(name);
	while (shell_context->envp_cpy[i])
	{
		if (ft_strncmp(shell_context->envp_cpy[i], name, len) == 0 && shell_context->envp_cpy[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

static int	add_new_env_var(const char *arg, t_shell_context *shell_context)
{
	char	**new_envp;
	int		count;
	int		i;

	count = 0;
	while ((shell_context->envp_cpy)[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (1);
	i = -1;
	while ((shell_context->envp_cpy)[++i])
	{
		new_envp[i] = (shell_context->envp_cpy)[i];
		(shell_context->envp_cpy)[i] = NULL;
	}
	new_envp[i] = ft_strdup(arg);
	new_envp[i + 1] = NULL;
	free(shell_context->envp_cpy);
	shell_context->envp_cpy = new_envp;
	return (0);
}

int	builtin_export(char **args, t_shell_context *shell_context)
{
	int		i;
	char	*var_name;
	char	*eq_ptr;
	int		idx;

	if (!args[1])
		return (builtin_env(shell_context->envp_cpy));
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
		idx = find_env_var_index(var_name, shell_context);
		if (idx != -1 && eq_ptr)
		{
			free((shell_context->envp_cpy)[idx]);
			(shell_context->envp_cpy)[idx] = ft_strdup(args[i]);
		}
		else if (idx == -1 && eq_ptr)
			add_new_env_var(args[i], shell_context);
		free(var_name);
		i++;
	}
	return (0);
}

int	builtin_unset(char **args, t_shell_context *shell_context)
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
		idx = find_env_var_index(args[i], shell_context);
		if (idx != -1)
		{
			var_to_remove = (shell_context->envp_cpy)[idx];
			while ((shell_context->envp_cpy)[idx])
			{
				(shell_context->envp_cpy)[idx] = (shell_context->envp_cpy)[idx + 1];
				idx++;
			}
			free(var_to_remove);
		}
		i++;
	}
	return (0);
}
