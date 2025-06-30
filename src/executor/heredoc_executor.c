#include "../../include/minishell.h"

static int	create_heredoc_pipe(int *pipe_fd)
{
	if (pipe(pipe_fd) == -1)
	{
		perror("minishell: pipe");
		return (1);
	}
	return (0);
}
/*
 * Propósito: Crear un par de descriptores para here-doc.
 * Mecanismo:
 *   1. Llama a `pipe(pipe_fd)`.
 *   2. Si falla, imprime errno y retorna 1.
 *   3. Si tiene éxito, retorna 0.
 * Llamado por: `execute_heredoc`.
 * Llama a:
 *   - `pipe`
 *   - `perror`
 */

static void	write_line_to_pipe(int write_fd, char *line)
{
	if (!line)
		return ;
	write(write_fd, line, ft_strlen(line));
	write(write_fd, "\n", 1);
}
/*
 * Propósito: Escribir una línea seguida de '\n' en el fd de here-doc.
 * Mecanismo:
 *   1. Si `line` es NULL, retorna.
 *   2. Llama a `write` para la línea y luego escribe el salto de línea.
 * Llamado por: `read_heredoc_input`.
 * Llama a:
 *   - `write`
 *   - `ft_strlen`
 */

static int	read_heredoc_input(char *delimiter, int write_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		write_line_to_pipe(write_fd, line);
		free(line);
	}
	return (0);
}
/*
 * Propósito: Leer líneas hasta encontrar el delimitador y volcar al pipe.
 * Mecanismo:
 *   1. Bucle infinito llamando a `readline("> ")`.
 *   2. Si `line` es NULL o coincide con `delimiter`, sale del bucle.
 *   3. En caso contrario, escribe la línea en `write_fd` y repite.
 *   4. Libera cada `line` después de usarla.
 * Llamado por: `execute_heredoc`.
 * Llama a:
 *   - `readline`
 *   - `ft_strcmp`
 *   - `write_line_to_pipe`
 *   - `free`
 */

int	execute_heredoc(char *delimiter)
{
	int	pipe_fd[2];

	if (create_heredoc_pipe(pipe_fd) != 0)
		return (-1);
	read_heredoc_input(delimiter, pipe_fd[1]);
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}
/*
 * Propósito: Ejecutar la rutina de here-doc y devolver el fd de lectura.
 * Mecanismo:
 *   1. Crea un pipe con `create_heredoc_pipe`.
 *   2. Llama a `read_heredoc_input` para capturar líneas hasta `delimiter`.
 *   3. Cierra el extremo de escritura.
 *   4. Retorna el extremo de lectura para usarlo como STDIN.
 * Llamado por: `parse_redirect_executor` y `search_heredoc_recursive`.
 * Llama a:
 *   - `create_heredoc_pipe`
 *   - `read_heredoc_input`
 *   - `close`
 */
