#include "../../include/minishell.h"

// static int	cosecutive_ops_checker(t_token *tokens)
// {
// 	t_token *curr = tokens;

// 	while (curr && curr->next)
//     {
//         // Detecta dos operadores consecutivos (no permitidos)
//         // PERO PERMITE: >|, |>, >>|, |>>, <|, |<, etc.
//         if (curr->type != TOKEN_WORD && curr->type != TOKEN_EOF
//             && curr->next->type != TOKEN_WORD && curr->next->type != TOKEN_EOF)
//         {
//             // Permitir redirección seguida de pipe y viceversa
//             if ((is_redirect_token(curr->type) && curr->next->type == TOKEN_PIPE) ||
//                 (curr->type == TOKEN_PIPE && is_redirect_token(curr->next->type)))
//             {
//                 curr = curr->next;
//                 continue;
//             }
//             ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);
//             if (curr->next->type == TOKEN_EOF)
//                 ft_putstr_fd("newline", STDERR_FILENO);
//             else if (curr->next->value)
//                 ft_putstr_fd(curr->next->value, STDERR_FILENO);
//             else
//                 ft_putstr_fd("?", STDERR_FILENO);
//             ft_putstr_fd("'\n", STDERR_FILENO);
//             return (1);
//         }
// 		curr = curr->next;
// 	}
// 	return (0);
// }
// static int	extremes_ops_checker(t_token *tokens)
// {
// 	t_token	*last;

// 	if (tokens->next->type == 10 && (tokens->type == 0 || tokens->type == 8))
// 		return (0);
// 	if (tokens->next->type == 10 && (tokens->type > 0 && tokens->type < 10))
// 	{
// 		ft_putstr_fd("minishell: syntax error near unexpected token '",
// 				STDERR_FILENO);
// 		ft_putstr_fd(tokens->value, STDERR_FILENO);
// 		return (ft_putstr_fd("'\n", STDERR_FILENO), 1); // Debo liberar las listas antes de retornar?
// 	}
// 	last = tokens;
// 	while (last->next->next != NULL)
// 		last = last->next;
// 	if (tokens->type == 1 || tokens->type == 6 || last->type == 1)
// 	{
// 		ft_putstr_fd("minishell: syntax error near unexpected token '",
// 				STDERR_FILENO);
// 		if ((tokens->type == 1 || tokens->type == 6) && last->type != 1)
// 			ft_putstr_fd(tokens->value, STDERR_FILENO);
// 		else
// 			ft_putstr_fd(last->value, STDERR_FILENO);
// 		return (ft_putstr_fd("'\n", STDERR_FILENO), 1); // Debo liberar las listas antes de retornar?
// 	}
// 	return (0);
// }

int	run_shell_loop(t_shell_context *shell_context)
{
	char	*input;
	char	*temp;
	char	*full_input;
	int		exit_code;
//	int		should_exit;
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
