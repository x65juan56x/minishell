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
		ft_putendl_fd(line, write_fd);
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
	int		pipe_fd[2];
	int		status;
	int		stdin_backup;
	pid_t	pid;
	struct termios	orig_termios;

	tcgetattr(STDIN_FILENO, &orig_termios);
	if (create_heredoc_pipe(pipe_fd) != 0)
		return (-1);
	stdin_backup = dup(STDIN_FILENO); // 1. Guardar el stdin original
	if (stdin_backup < 0)
		return (perror("dup"), -1);
	// Ignorar SIGINT en el padre mientras el hijo lee el heredoc
	signals_ignored();
	pid = fork();
	if (pid == -1)
		return (perror("fork"), close(stdin_backup), -1);
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		// Proceso hijo: lee la entrada y escribe en el pipe
		close(stdin_backup); // El hijo no necesita el backup
		close(pipe_fd[0]); // El hijo no lee del pipe
		read_heredoc_input(delimiter, pipe_fd[1]);
		close(pipe_fd[1]);
		exit(0);
	}
	// Proceso padre: espera al hijo y gestiona los descriptores
	signal(SIGINT, SIG_IGN);
	close(pipe_fd[1]);
	waitpid(pid, &status, 0);
	signal(SIGINT, sigint_handler);
	dup2(stdin_backup, STDIN_FILENO); // 2. Restaurar el stdin original
	close(stdin_backup);
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
		{
			close(pipe_fd[0]);
			write(1, "\n", 1);
			return (-2); // Señal: heredoc cancelado
		}
	}
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
