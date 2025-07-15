#include "../../include/minishell.h"

static void	heredoc_child_routine(const char *filename, char *delimiter)
{
	setup_heredoc_signals();
	if (create_heredoc_file(filename, delimiter) != 0)
		exit(1); // Sale con error si la creación del fichero falla.
	exit(0); // Éxito
}

static int	heredoc_parent_routine(pid_t pid, struct termios *orig_termios)
{
	int	status;

	waitpid(pid, &status, 0);
	tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
	if (WIFSIGNALED(status) || (WIFEXITED(status)
		&& WEXITSTATUS(status) == 130))
	{
		g_signal_status = SIGINT;
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		return (-1);
	return (0);
}

int	execute_heredoc(const char *filename, char *delimiter)
{
	pid_t			pid;
	struct termios	orig_termios;

	disable_ctrl_echo(&orig_termios);
	ignore_signals();
	pid = fork();
	if (pid == -1)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		heredoc_child_routine(filename, delimiter);
	return (heredoc_parent_routine(pid, &orig_termios));
}
