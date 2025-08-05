#include "../../include/minishell.h"

// Función de debug temporal para shell_loop
int	run_shell_loop_debug(t_shell_context *shell_context)
{
	char	*input;
	int		multiline_status;
	int		iteration = 0;

	while (1)
	{
		printf("DEBUG: Iteration %d, g_signal_status = %d\n", ++iteration, g_signal_status);
		setup_interactive_signals();
		printf("DEBUG: Calling get_user_input()\n");
		input = get_user_input();
		printf("DEBUG: get_user_input() returned: [%s], g_signal_status = %d\n", 
			   input ? input : "NULL", g_signal_status);
		
		if (!input)
			break ;
		
		if (g_signal_status == SIGINT)
		{
			printf("DEBUG: SIGINT detected, resetting and continuing\n");
			shell_context->exit_status = 130;
			free(input);
			g_signal_status = 0;
			continue ;
		}
		
		printf("DEBUG: Calling handle_multiline_input()\n");
		multiline_status = handle_multiline_input(&input);
		printf("DEBUG: handle_multiline_input() returned: %d, input = [%s]\n", 
			   multiline_status, input ? input : "NULL");
		
		if (multiline_status == 1)
			return (1);
		
		printf("DEBUG: Calling process_input()\n");
		process_input(input, shell_context);
		printf("DEBUG: process_input() completed\n");
		
		free(input);
		if (shell_context->should_exit)
			break ;
	}
	return (shell_context->exit_status);
}
