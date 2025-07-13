#include "../../include/minishell.h"

static void	heredoc_child_routine(char *delimiter, int *heredoc_id_ptr)
{
	char	*temp_filename;

	setup_heredoc_signals();
	temp_filename = create_heredoc_temp_file(delimiter, *heredoc_id_ptr);
	if (!temp_filename)
		exit(1);
	free(temp_filename);
	exit(0);
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
		return (-2);
	}
	return (0);
}

int	execute_heredoc(char *delimiter, int *heredoc_id_ptr)
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
		heredoc_child_routine(delimiter, heredoc_id_ptr);
	return (heredoc_parent_routine(pid, &orig_termios));
}
