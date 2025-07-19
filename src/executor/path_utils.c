#include "../../include/minishell.h"

static char	*get_path_from_env(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (NULL);
	return (envp[i] + 5);
}
/*
 * Propósito: Extraer el valor de la variable de entorno PATH.
 * Mecanismo:
 *   1. Comprueba que el array `envp` no sea NULL.
 *   2. Itera con índice `i` sobre cada cadena de `envp`.
 *   3. Usa `ft_strncmp` para buscar la entrada que comienza con "PATH=".
 *   4. Si la encuentra, devuelve el puntero al carácter justo después de "PATH=",
 *      que corresponde al inicio de la lista de directorios.
 *   5. Si no la encuentra o `envp` es NULL, devuelve NULL.
 * Llamado por: find_command_path
 * Llama a:    ft_strncmp
 */

static char	*build_full_path(char *dir, char *cmd)
{
	char	*path_with_slash;
	char	*full_path;

	path_with_slash = ft_strjoin(dir, "/");
	if (!path_with_slash)
		return ((char *)-1);
	full_path = ft_strjoin(path_with_slash, cmd);
	free(path_with_slash);
	if (!full_path)
		return ((char *)-1);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}
/*
 * Propósito: Construir y verificar la ruta absoluta de un ejecutable.
 * Mecanismo:
 *   1. Concatena `dir` + "/" en `with_slash` usando `ft_strjoin`.
 *   2. Concatena `with_slash` + `cmd` en `full`.
 *   3. Libera `with_slash`.
 *   4. Comprueba con `access(full, X_OK)` si el archivo existe y es ejecutable.
 *   5. Si es ejecutable, devuelve `full`. Si no, libera `full` y devuelve NULL.
 *   6. Si falla cualquier `malloc`, devuelve (char *)-1 para señal de error.
 * Llamado por: search_cmd_in_dirs
 * Llama a:    ft_strjoin, free, access
 */

static char	*search_cmd_in_dirs(char **dirs, char *cmd)
{
	char	*path;
	int		i;

	i = -1;
	while (dirs[++i])
	{
		path = build_full_path(dirs[i], cmd);
		if (path == (char *)-1)
			return (ft_freearr(dirs), NULL);
		if (path != NULL)
			return (ft_freearr(dirs), path);
	}
	ft_freearr(dirs);
	return (NULL);
}
/*
 * Propósito: Buscar un comando ejecutable en una lista de directorios.
 * Mecanismo:
 *   1. Recorre el array `dirs` hasta el último elemento NULL.
 *   2. En cada iteración, llama a `build_full_path(dirs[i], cmd)`.
 *   3. Si `build_full_path` retorna (char *)-1, libera `dirs` y retorna NULL.
 *   4. Si retorna una ruta válida, libera `dirs` y la devuelve.
 *   5. Si no encuentra ninguna ruta válida, libera `dirs` y retorna NULL.
 * Llamado por: find_command_path
 * Llama a:    build_full_path, ft_freearr
 */

static int	check_direct_path(char *cmd)
{
	if (access(cmd, F_OK) != 0)
		return (127); //Archivo no existe
	if (access(cmd, X_OK) != 0)
		return (126); //Archivo existe pero no tiene permiso de ejecución
	return (0); //Todo OK
}

char	*find_command_path(char *cmd, char **envp)
{
	char	*path;
	char	**dirs;
	char	*tmp;
	int		status;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/') != NULL)
	{
		status = check_direct_path(cmd);
		if (status == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	tmp = get_path_from_env(envp);
	if (!tmp)
		return (NULL);
	dirs = ft_split(tmp, ':');
	if (!dirs)
		return (NULL);
	path = search_cmd_in_dirs(dirs, cmd);
	return (path);
}
/*
 * Propósito: Resolver la ruta absoluta de un comando según PATH o ruta directa.
 * Mecanismo:
 *   1. Si `cmd` contiene '/', trata `cmd` como ruta y comprueba `access`.
 *   2. Extrae el valor PATH con `get_path_from_env`; si es NULL, usa una ruta
 *      por defecto predefinida.
 *   3. Divide PATH en directorios con `ft_split`.
 *   4. Llama a `search_cmd_in_dirs` para encontrar el ejecutable en esos dirs.
 *   5. Devuelve la ruta encontrada o NULL si no la localiza.
 * Llamado por: launch_command
 * Llama a:    ft_strchr, access, ft_strdup, get_path_from_env, ft_split,
 *              search_cmd_in_dirs
 */
