#include "../../include/minishell.h"

int	run_shell_loop(char ***envp_ptr)
{
	char	*input;
	int		exit_status;
	int		should_exit;

	exit_status = 0;
	while (1)
	{
		// Configurar señales para el modo interactivo al inicio de cada bucle.
		// Esto asegura que el shell siempre esté listo para recibir entrada.
		setup_interactive_signals();
		g_signal_status = 0; // Resetear el estado de la señal global.
		input = get_user_input();
		if (!input)
			break ; // Ctrl+D presionado
		// Si Ctrl+C fue presionado durante readline, g_signal_status será SIGINT.
		if (g_signal_status == SIGINT)
			exit_status = 130;
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
