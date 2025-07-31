#include "../../include/minishell.h"

char	*get_env_value(const char *var_name, t_shell_context *shell_context)
{
	int		i;
	size_t	len;
	t_list	*current;
	char	*content;
	char	*eq_ptr;

	if (!var_name || !shell_context || !shell_context->envp_cpy)
		return (NULL);
	len = ft_strlen(var_name);
	current = shell_context->local_vars;
	while (current)
	{
		content = (char *)current->content;
		eq_ptr = ft_strchr(content, '=');
		if (eq_ptr && (size_t)(eq_ptr - content) == len
			&& ft_strncmp(content, var_name, len) == 0)
			return (eq_ptr + 1);
		current = current->next;
	}
	i = -1;
	while (shell_context->envp_cpy && shell_context->envp_cpy[++i])
		if (ft_strncmp(shell_context->envp_cpy[i], var_name, len) == 0
			&& shell_context->envp_cpy[i][len] == '=')
			return (shell_context->envp_cpy[i] + len + 1);
	return (NULL);
}
// Función auxiliar para obtener el valor de una variable de nuestro entorno.
// No usamos getenv() porque debemos leer de nuestra copia privada.
// Busca primero en las variables locales
// Luego busca en el env_cpy

int	is_valid_identifier(const char *name)
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

int	find_env_var_index(const char *name, t_shell_context *shell_context)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(name);
	while (shell_context->envp_cpy[i])
	{
		if (ft_strncmp(shell_context->envp_cpy[i], name, len) == 0
			&& shell_context->envp_cpy[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

static char	**create_sorted_array(char **envp_cpy, int count)
{
	char	**sorted_envp;
	int		i;

	sorted_envp = malloc(sizeof(char *) * (count + 1));
	if (!sorted_envp)
		return (NULL);
	i = -1;
	while (++i < count)
		sorted_envp[i] = envp_cpy[i];
	sorted_envp[count] = NULL;
	return (sorted_envp);
}

static void	bubble_sort_array(char **sorted_envp, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = -1;
	while (++i < count - 1)
	{
		j = -1;
		while (++j < count - 1 - i)
		{
			if (ft_strcmp(sorted_envp[j], sorted_envp[j + 1]) > 0)
			{
				temp = sorted_envp[j];
				sorted_envp[j] = sorted_envp[j + 1];
				sorted_envp[j + 1] = temp;
			}
		}
	}
}

static void	print_export_var(char *env_var)
{
	char	*temp;
	char	*name;
	char	*value;

	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	temp = ft_strchr(env_var, '=');
	if (temp)
	{
		name = ft_substr(env_var, 0, temp - env_var);
		value = temp + 1;
		ft_putstr_fd(name, STDOUT_FILENO);
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(value, STDOUT_FILENO);
		ft_putendl_fd("\"", STDOUT_FILENO);
		free(name);
	}
}

static void	print_sorted_export(char **sorted_envp, int count)
{
	int	i;

	i = -1;
	while (++i < count)
		print_export_var(sorted_envp[i]);
}

void	sort_and_print_export(char **envp_cpy)
{
	char	**sorted_envp;
	int		count;

	count = 0;
	while (envp_cpy[count])
		count++;
	sorted_envp = create_sorted_array(envp_cpy, count);
	if (!sorted_envp)
		return;
	bubble_sort_array(sorted_envp, count);
	print_sorted_export(sorted_envp, count);
	free(sorted_envp);
}

int	add_new_env_var(const char *arg, t_shell_context *shell_context)
{
	int		count;
	char	**new_envp;
	int		i;

	count = 0;
	while (shell_context->envp_cpy[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (1);
	i = -1;
	while (++i < count)
	{
		new_envp[i] = ft_strdup(shell_context->envp_cpy[i]);
		if (!new_envp[i])
			return (ft_freearr(new_envp), 1);
	}
	new_envp[i] = ft_strdup(arg);
	if (!new_envp[i])
		return (ft_freearr(new_envp), 1);
	new_envp[i + 1] = NULL;
	ft_freearr(shell_context->envp_cpy);
	shell_context->envp_cpy = new_envp;
	return (0);
}

void	remove_local_var(const char *name, t_list **local_vars)
{
	t_list	*prev;
	t_list	*current;
	size_t	name_len;
	char	*content;

	prev = NULL;
	current = *local_vars;
	name_len = ft_strlen(name);
	while (current)
	{
		content = (char *)current->content;
		if (ft_strncmp(content, name, name_len) == 0
			&& (content[name_len] == '=' || content[name_len] == '\0'))
		{
			if (prev)
				prev->next = current->next;
			else
				*local_vars = current->next;
			ft_lstdelone(current, free);
			return ;
		}
		prev = current;
		current = current->next;
	}
}
