#include "../include/minishell.h"

int	main(int ac, char **av, char **envp)
{
	char		**envp_copy;
	int			exit_status;

	(void)ac;
	(void)av;
	envp_copy = init_shell_environment(envp);
	exit_status = run_shell_loop(&envp_copy);
	ft_freearr(envp_copy);
	rl_clear_history();
	return (exit_status);
}
