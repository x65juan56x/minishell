#include "../../include/minishell.h"

int	run_shell_loop(char ***envp_ptr)
{
	char	*input;
	int		exit_status;
	int		should_exit;

	exit_status = 0;
	while (1)
	{
		input = get_user_input();
		if (!input)
			break ;
		
		should_exit = handle_input_line(input);
		if (should_exit)
		{
			free(input);
			break ;
		}
		
		if (*input)
			exit_status = process_command_line(input, envp_ptr);
		
		free(input);
	}
	return (exit_status);
}
