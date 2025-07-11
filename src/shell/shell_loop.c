#include "../../include/minishell.h"

int	run_shell_loop(char ***envp_ptr)
{
	char	*input;
	int		exit_status;
	char	*temp;
	char	*full_input;

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

		while (are_quotes_unclosed(input))
		{
			// Pedimos la siguiente línea con un prompt secundario
			full_input = readline("> ");
			if (!full_input) // Ctrl+D en medio de la entrada
			{
				ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
				free(input);
				input = ft_strdup(""); // Evita que se procese la línea rota
				break;
			}
			// Unimos la línea anterior con la nueva (añadiendo un \n)
			temp = ft_strjoin(input, "\n");
			free(input);
			input = ft_strjoin(temp, full_input);
			free(temp);
			free(full_input);
		}
		// Si Ctrl+C fue presionado durante readline, g_signal_status será SIGINT.
		if (g_signal_status == SIGINT)
			exit_status = 130;
		if (handle_input_line(input))
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
