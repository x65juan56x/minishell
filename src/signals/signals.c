#include "../../include/minishell.h"

void	signals_parent()
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	rl_catch_signals = 0;
}

void	signals_ignored()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void	signals_default()
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	sigint_handler(int signum)
{
	//signal(SIGINT, SIG_IGN);
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	(void)signum;
}


//tengo dudas sobre la gestion de la memoria en la finalización de las señales (????)