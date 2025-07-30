#include "../../include/minishell.h"

int	run_shell_loop(t_shell_context *shell_context)
{
	char	*input;
	char	*temp;
	char	*full_input;
	int		exit_code;
	t_token	*tokens;

	while (1)
	{
		setup_interactive_signals(); // Configurar señales para el modo interactivo al inicio de cada bucle. Esto asegura que el shell siempre esté listo para recibir entrada.
		g_signal_status = 0; // Resetear el estado de la señal global.
		input = get_user_input();
		if (!input)
			break ; // Ctrl+D presionado

		while (are_quotes_unclosed(input) || are_parentheses_unclosed(input))
		{
			if (!isatty(STDIN_FILENO)) // No permitir multilínea en modo no interactivo
			{
				ft_putstr_fd("minishell: syntax error: unexpected end of file\n", STDERR_FILENO);
				free(input);
				input = ft_strdup(""); // Línea vacía para evitar procesar la rota
				break;
			}
			full_input = readline("> ");
			if (!full_input) // Ctrl+D en prompt secundario
			{
				ft_putstr_fd("minishell: syntax error: unexpected end of file\n", STDERR_FILENO);
				free(input);
				input = ft_strdup("");
				break;
			}
			// Unir la línea anterior con la nueva, separadas por un espacio
			temp = ft_strjoin(input, " ");
			free(input);
			input = ft_strjoin(temp, full_input);
			free(temp);
			free(full_input);
			if (!input) // Fallo de malloc
				return (1);
		}

		if (g_signal_status == SIGINT)
			shell_context->exit_status = 130;
		
		if (*input) // Solo procesar si la línea no está vacía
		{
			handle_input_line(input); // Añadir al historial
			tokens = tokenize(input);
			if (tokens)
            {
                // --- AÑADIR ESTA LÓGICA DE SALIDA CONTROLADA ---
                // Comprobamos si el comando es 'exit' y no está en un pipe o subshell.
                // Esta es una forma simple de detectar si debe terminar el shell principal.
                if (tokens->type == TOKEN_WORD && ft_strcmp(tokens->value, "exit") == 0)
                {
                    // Si hay algo más que 'exit' y sus argumentos (como un pipe),
                    // el ejecutor lo manejará en un subproceso.
                    // Aquí solo manejamos el caso en que 'exit' debe terminar el shell.
                    t_token *next_token = tokens->next;
                    while (next_token && next_token->type == TOKEN_WORD)
                        next_token = next_token->next;
                    
                    if (next_token->type == TOKEN_EOF)
                    {
//                      ft_putendl_fd("exit", STDOUT_FILENO);
                        // Llamamos a una versión "falsa" de exit que solo calcula el status
                        // y no termina el proceso, para devolverlo al main.
                        exit_code = get_exit_status_from_args(tokens->next);
                        cleanup_tokens(tokens);
                        free(input);
                        return (exit_code); // Devolvemos el status para que main() termine.
                    }
                }
                exit_code = process_command_line(tokens, shell_context);
                shell_context->exit_status = exit_code;
            }
			else // Tokenize falló (debería ser raro)
				shell_context->exit_status = 2;
		}
		free(input);
	}
	return (shell_context->exit_status);
}
