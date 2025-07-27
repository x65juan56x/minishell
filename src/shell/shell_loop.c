#include "../../include/minishell.h"

static int	cosecutive_ops_checker(t_token *tokens)
{
	t_token *curr = tokens;

	while (curr && curr->next)
    {
        // Detecta dos operadores consecutivos (no permitidos)
        // PERO PERMITE: >|, |>, >>|, |>>, <|, |<, etc.
        if (curr->type != TOKEN_WORD && curr->type != TOKEN_EOF
            && curr->next->type != TOKEN_WORD && curr->next->type != TOKEN_EOF)
        {
            // Permitir redirección seguida de pipe y viceversa
            if ((is_redirect_token(curr->type) && curr->next->type == TOKEN_PIPE) ||
                (curr->type == TOKEN_PIPE && is_redirect_token(curr->next->type)))
            {
                curr = curr->next;
                continue;
            }
            ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);
            if (curr->next->type == TOKEN_EOF)
                ft_putstr_fd("newline", STDERR_FILENO);
            else if (curr->next->value)
                ft_putstr_fd(curr->next->value, STDERR_FILENO);
            else
                ft_putstr_fd("?", STDERR_FILENO);
            ft_putstr_fd("'\n", STDERR_FILENO);
            return (1);
        }
		curr = curr->next;
	}
	return (0);
}
static int	extremes_ops_checker(t_token *tokens)
{
	t_token	*last;

	if (tokens->next->type == 10 && (tokens->type == 0 || tokens->type == 8))
		return (0);
	if (tokens->next->type == 10 && (tokens->type > 0 && tokens->type < 10))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '",
				STDERR_FILENO);
		ft_putstr_fd(tokens->value, STDERR_FILENO);
		return (ft_putstr_fd("'\n", STDERR_FILENO), 1); // Debo liberar las listas antes de retornar?
	}
	last = tokens;
	while (last->next->next != NULL)
		last = last->next;
	if (tokens->type == 1 || tokens->type == 6 || last->type == 1)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '",
				STDERR_FILENO);
		if ((tokens->type == 1 || tokens->type == 6) && last->type != 1)
			ft_putstr_fd(tokens->value, STDERR_FILENO);
		else
			ft_putstr_fd(last->value, STDERR_FILENO);
		return (ft_putstr_fd("'\n", STDERR_FILENO), 1); // Debo liberar las listas antes de retornar?
	}
	return (0);
}

int	run_shell_loop(t_shell_context *shell_context)
{
	char	*input;
	char	*temp;
	char	*full_input;
	int		exit_code;
	int		should_exit;
	t_token	*tokens;

	while (1)
	{
		setup_interactive_signals(); // Configurar señales para el modo interactivo al inicio de cada bucle. Esto asegura que el shell siempre esté listo para recibir entrada.
		g_signal_status = 0; // Resetear el estado de la señal global.
		input = get_user_input();
		if (!input)
			break ; // Ctrl+D presionado

		tokens = tokenize(input);
		if (!tokens)
			return (1);
//		debug_print_token_list(tokens);//DEBUG
		if (extremes_ops_checker(tokens) || cosecutive_ops_checker(tokens))
			return (cleanup_tokens(tokens), 2);

		while (are_quotes_unclosed(input) || is_lparen_and_or_open(input))
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
			else if (is_lparen_and_or_open(input))
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
			exit_code = process_command_line(tokens, shell_context);
			if (exit_code <= -2) // Solo salir si el usuario pidió exit
			{
				// Si exit_code == -2, status 0. Si exit_code == -2 - N, status N.
				if (exit_code == -2)
					shell_context->exit_status = 0;
				else
					shell_context->exit_status = -(exit_code + 2);
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
				break ;
			}
		}
		free(input);
	}
	return (shell_context->exit_status);
}
