#include "../../include/minishell.h"

// void	disable_ctrl_echo(struct termios *orig_termios)
// {
// 	struct termios	new_termios;

// 	if (tcgetattr(STDIN_FILENO, orig_termios) == -1)
// 		return ;
// 	new_termios = *orig_termios;
// 	new_termios.c_lflag &= ~ECHOCTL; // Desactivar el bit ECHOCTL
// 	if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) == -1)
// 		return ;
// }
/**
 * Desactiva el eco de caracteres de control (ej. ^C) en la terminal.
 * orig_termios Puntero para guardar la configuración original.
 */

int	read_heredoc_input(char *delimiter, int write_fd)
{
	char	*line;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			line = readline("> ");
		else
			line = ft_mini_gnl_char(STDIN_FILENO);
		if (!line)
		{
			if (isatty(STDIN_FILENO))
				ft_putstr_fd("minishell: warning: here-docu", STDERR_FILENO);
			if (isatty(STDIN_FILENO))
				ft_putstr_fd("ment delimited by end-of-file\n", STDERR_FILENO);
			return (1);
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
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

// Esta función abre y escribe en un fichero cuyo nombre se le pasa.
// Devuelve 0 en éxito, -1 en error.
int	create_heredoc_file(const char *filename, char *delimiter)
{
	int	fd;

	fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0600);
	if (fd < 0)
		return (perror("minishell: heredoc"), -1);
	read_heredoc_input(delimiter, fd); // read_heredoc_input se encarga de leer del usuario y escribir en el fd.
	close(fd);
	return (0);
}
