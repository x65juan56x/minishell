#include "../../include/minishell.h"

// Función auxiliar para actualizar una variable de entorno o crearla si no existe.
// Es una versión simplificada de `export` para uso interno.
static int	update_env_var(const char *var_name, const char *value,
							char ***envp_ptr)
{
	char	*new_var;
	char	*temp;
	char	*export_args[3];

	temp = ft_strjoin(var_name, "=");
	if (!temp)
		return (1);
	new_var = ft_strjoin(temp, value);
	free(temp);
	if (!new_var)
		return (1);
	// Usamos la misma lógica de export: buscar y reemplazar, o añadir.
	// Esto requiere que `builtin_export` sea accesible o duplicar la lógica.
	// Para simplicidad, asumimos que podemos llamar a una función que lo haga.
	// Aquí simulamos la llamada a export con un solo argumento. (no la llamamos desde el ejecutor sino que la usamos directamente como auxiliar)
	export_args[0] = "export";
	export_args[1] = new_var;
	export_args[2] = NULL;
	builtin_export(export_args, envp_ptr);
	free(new_var);
	return (0);
}

// Función auxiliar para obtener el valor de una variable de nuestro entorno.
// No usamos getenv() porque debemos leer de nuestra copia privada.
static char	*get_env_value(const char *var_name, char **envp)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(var_name);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

static int	go_to_path(const char *path, char ***envp_ptr)
{
	char	*old_pwd;
	char	new_pwd[1024];

	old_pwd = get_env_value("PWD", *envp_ptr);
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	if (old_pwd)
		update_env_var("OLDPWD", old_pwd, envp_ptr);
	if (getcwd(new_pwd, sizeof(new_pwd)) != NULL)
		update_env_var("PWD", new_pwd, envp_ptr);
	else
	{
		perror("minishell: cd: getcwd");
		return (1);
	}
	return (0);
}

int	builtin_cd(char **args, char ***envp_ptr)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_value("HOME", *envp_ptr);
		if (!path)
			return (ft_putendl_fd("minishell: cd: HOME not set", 2), 1);
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value("OLDPWD", *envp_ptr);
		if (!path)
			return (ft_putendl_fd("minishell: cd: OLDPWD not set", 2), 1);
		ft_putendl_fd(path, STDOUT_FILENO); // `cd -` imprime el directorio al que va
	}
	else
		path = args[1];
	return (go_to_path(path, envp_ptr));
}
