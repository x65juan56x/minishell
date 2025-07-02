#include "../../include/minishell.h"

static void	exit_command_not_found(char **args, char *path)
{
	char	*cmd_name;

	cmd_name = ft_strdup(args[0]);
	if (path)
		free(path);
	ft_freearr(args);
	ft_cmd_not_found_exit(cmd_name);
	free(cmd_name);
}
/*
 * Propósito: Salir con código 127 cuando el comando no existe.
 * Mecanismo:
 *   1. Duplica el nombre de comando (`args[0]`) para el mensaje de error.
 *   2. Libera `path` si se había reservado.
 *   3. Libera el array de argumentos con `ft_freearr`.
 *   4. Llama a `ft_cmd_not_found_exit` para imprimir “command not found” y salir.
 *   5. Libera la copia local `cmd_name`.
 * Llamado por: `launch_command` cuando `find_command_path` devuelve NULL.
 * Llama a:
 *   - `ft_strdup`
 *   - `free`
 *   - `ft_freearr`
 *   - `ft_cmd_not_found_exit`
 */

static void	exit_execve_error(char **args, char *path)
{
	char	*cmd_name;

	cmd_name = ft_strdup(args[0]);
	free(path);
	ft_freearr(args);
	ft_execve_error_exit(cmd_name);
	free(cmd_name);
}
/*
 * Propósito: Salir con código 126 cuando `execve` falla.
 * Mecanismo:
 *   1. Duplica el nombre de comando para el mensaje de error.
 *   2. Libera la ruta `path`.
 *   3. Libera el array de argumentos con `ft_freearr`.
 *   4. Llama a `ft_execve_error_exit` que imprime el error de `errno` y sale.
 *   5. Libera la copia local `cmd_name`.
 * Llamado por: `launch_command` si `execve` retorna.
 * Llama a:
 *   - `ft_strdup`
 *   - `free`
 *   - `ft_freearr`
 *   - `ft_execve_error_exit`
 */

void	launch_command(char **args, char **envp)
{
	char	*path;

	printf("[cmd_executor] launch_command: %s\n", args[0]);
	if (!args || !args[0] || args[0][0] == '\0')
		ft_cmd_not_found_exit("");
	path = find_command_path(args[0], envp);
	if (!path)
		exit_command_not_found(args, path);
	execve(path, args, envp);
	exit_execve_error(args, path);
}
/*
 * Propósito: Resolver y ejecutar un comando externo.
 * Mecanismo:
 *   1. Verifica que `args` y `args[0]` sean válidos; si no, sale 127.
 *   2. Llama a `find_command_path` para obtener la ruta absoluta.
 *   3. Si no existe, invoca `exit_command_not_found`.
 *   4. Llama a `execve(path, args, envp)`.
 *   5. Si `execve` retorna, invoca `exit_execve_error`.
 * Llamado por: Procesos hijos que deben ejecutar un `NODE_COMMAND`.
 * Llama a:
 *   - `find_command_path`
 *   - `execve`
 *   - `exit_command_not_found`
 *   - `exit_execve_error`
 */
