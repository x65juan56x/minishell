#include "konxitah.h"

int	main(int ac, char **av, char **envp)
{
	char    *input;
	pid_t	pid;

	(void)ac;
	(void)av;
	while (1)
	{
		input = readline("🐚 ");
		if (!input)
		{
			printf("\n");
			break;
		}
		if (*input)
			add_history(input);
		if (ft_strcmp(input, "exit") == 0)
		{
			free(input);
			break;
		}
		pid = fork();
		if (pid == 0)
		{
			run_cmd(input, envp);
			exit(1);
		}
		else if (pid > 0)
			waitpid(pid, NULL, 0);
		free(input);
	}
	rl_clear_history();
	return (0);
}
