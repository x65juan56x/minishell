#include "../../include/minishell.h"

void	signals()
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	rl_catch_signals = 0;
}

void	sigint_handler(int signum)
{
	//signal(SIGINT, SIG_IGN);
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	(void)signum;
}
