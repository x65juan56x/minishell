#include "../../include/minishell.h"

int	run_shell_loop(t_shell_context *shell_context)
{
	char	*input;
	char	*temp;
	char	*full_input;
	int		exit_code;
	int		should_exit;

	while (1)
	{
		setup_interactive_signals(); // Configurar señales para el modo interactivo al inicio de cada bucle. Esto asegura que el shell siempre esté listo para recibir entrada.
		g_signal_status = 0; // Resetear el estado de la señal global.
		input = get_user_input();
		if (!input)
			break ; // Ctrl+D presionado
		while (are_quotes_unclosed(input) || is_and_open(input))
		{
			if (!isatty(STDIN_FILENO))  // Solo en modo interactivo
			{
				ft_putstr_fd("minishell: syntax error: unexpected end of file\n", STDERR_FILENO);
				free(input);
				input = ft_strdup("");
				break ;
			}
			full_input = readline("> "); // Pedimos la siguiente línea con un prompt secundario
			if (!full_input) // Ctrl+D en medio de la entrada
			{
				ft_putstr_fd("minishell: syntax error: unexpected end of file\n", STDERR_FILENO);
				free(input);
				input = ft_strdup(""); // Evita que se procese la línea rota
				break ;
			}
			if (are_quotes_unclosed(input))
				temp = ft_strjoin(input, "\n"); // Unimos la línea anterior con la nueva (añadiendo un \n)
			else if (is_and_open(input))
				temp = ft_strdup(input);
			free(input);
			if (!temp)
			{
				free(full_input);
				input = NULL;
				break ;
			}
			input = ft_strjoin(temp, full_input);
			free(temp);
			free(full_input);
			if (!input)
				break ;
		}
		if (!input)
			break ;
		if (g_signal_status == SIGINT) // Si Ctrl+C fue presionado durante readline, g_signal_status será SIGINT.
			shell_context->exit_status = 130;
		if (handle_input_line(input))
		{
			free(input);
			break ;
		}
		if (*input)
		{
			exit_code = process_command_line(input, shell_context);
			if (exit_code <= -2) // Solo salir si el usuario pidió exit
			{
				// Si exit_code == -2, status 0. Si exit_code == -2 - N, status N.
				shell_context->exit_status = (exit_code == -2) ? 0 : -(exit_code + 2);
				should_exit = 1;
			}
			else
			{
				shell_context->exit_status = exit_code;
				should_exit = 0;
			}
			if (should_exit)
			{
				free(input);
				break;
			}
		}
		free(input);
	}
	return (shell_context->exit_status);
}
