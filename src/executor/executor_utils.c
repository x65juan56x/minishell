#include "../../include/minishell.h"

void	print_signal_message(int signal_num)
{
	if (signal_num == SIGQUIT)
		ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
	else if (signal_num == SIGINT)
		ft_putstr_fd("\n", STDERR_FILENO);
}

int	analyze_child_status(int status)
{
	int	sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			g_signal_status = SIGINT;
		print_signal_message(sig);
		return (128 + sig);
	}
	return (status);
}