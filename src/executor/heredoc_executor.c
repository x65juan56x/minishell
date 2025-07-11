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

static void	disable_ctrl_echo(struct termios *orig_termios)
{
    struct termios	new_termios;

    if (tcgetattr(STDIN_FILENO, orig_termios) == -1)
        return ;
    new_termios = *orig_termios;
    new_termios.c_lflag &= ~ECHOCTL; // Desactivar el bit ECHOCTL
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) == -1)
        return ;
}
/**
 * @brief Desactiva el eco de caracteres de control (ej. ^C) en la terminal.
 * @param orig_termios Puntero para guardar la configuración original.
 */

static void	restore_ctrl_echo(struct termios *orig_termios)
{
    tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
}
/**
 *  Restaura la configuración original de la terminal.
 *  orig_termios Puntero a la configuración que se debe restaurar.
 */

int	execute_heredoc(char *delimiter)
{
	int				pipe_fd[2];
	int				status;
	pid_t			pid;
	struct termios	orig_termios;

	if (create_heredoc_pipe(pipe_fd) != 0)
		return (-1);
	disable_ctrl_echo(&orig_termios); // Ocultar ^C
    // El padre ignora las señales mientras el hijo del heredoc está activo.
    ignore_signals();
    pid = fork();
    if (pid == -1)
        return (perror("fork"), -1);
    if (pid == 0)
    {
        // El hijo del heredoc tiene un manejador especial para salir con código 130.
        setup_heredoc_signals();
        close(pipe_fd[0]);
        read_heredoc_input(delimiter, pipe_fd[1]);
        close(pipe_fd[1]);
        exit(0);
	}
    close(pipe_fd[1]);
    waitpid(pid, &status, 0);
    restore_ctrl_echo(&orig_termios); // 3. Restaurar la terminal sin importar el resultado
    // Si el hijo salió por una señal (Ctrl+C), WEXITSTATUS será 130.
    if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
    {
        close(pipe_fd[0]);
        g_signal_status = SIGINT; // Informar al bucle principal
        return (-2); // Código de error para heredoc cancelado
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