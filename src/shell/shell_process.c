#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens, t_ast_node *ast,
			t_shell_context *shell_context)
{
	if (tokens && tokens->type != TOKEN_EOF)
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
	if (shell_context->error_flag)
			return (2);
	if (ast)
		cleanup_ast(ast);
	if (tokens)
		cleanup_tokens(tokens);
	return (2);
}
static int	cosecutive_ops_checker(t_token *tokens)
{
	t_token *curr = tokens;

	while (curr && curr->next)
	{
		// Detecta dos operadores consecutivos (no permitidos)
		if (curr->type != TOKEN_WORD && curr->type != TOKEN_EOF
			&& curr->next->type != TOKEN_WORD && curr->next->type != TOKEN_EOF)
		{
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
    if (!tokens || !tokens->next)
        return (0);

    // Detecta si la línea comienza con operadores consecutivos
    if (tokens->type != TOKEN_WORD && tokens->type != TOKEN_EOF
        && tokens->next->type != TOKEN_WORD && tokens->next->type != TOKEN_EOF)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);
        if (tokens->next->value)
            ft_putstr_fd(tokens->next->value, STDERR_FILENO);
        else
            ft_putstr_fd("?", STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        return (1);
    }

    // Detecta si la línea termina con operador
    if (tokens->next->type == TOKEN_EOF && (tokens->type > 0 && tokens->type < 10))
    {
        ft_putstr_fd("minishell: syntax error near unexpected token '",
                STDERR_FILENO);
        ft_putstr_fd(tokens->value, STDERR_FILENO);
        ft_putstr_fd("'\n", STDERR_FILENO);
        return (1);
    }
    return (0);
}

int	process_command_line(char *input, t_shell_context *shell_context)
{
	t_token		*tokens;
	t_ast_node	*ast;
	int			exit_status;
	int			heredoc_id;

	heredoc_id = 0;
	shell_context->heredoc_files = NULL;
	tokens = tokenize(input);
	if (!tokens)
		return (1);
//	debug_print_token_list(tokens);//DEBUG
	if (extremes_ops_checker(tokens) || cosecutive_ops_checker(tokens))
		return (cleanup_tokens(tokens), 2);
	expander_var(tokens, shell_context);
	tokens = expand_wildcards(tokens);
// 	debug_print_token_list(tokens); //DEBUG
	ast = parse(tokens, shell_context);
	if (!ast)
		return (handle_parsing_error(tokens, ast, shell_context));
	//DEBUG
	// if (ast->type == NODE_COMMAND && ast->args)
	// {
	//     int i = 0;
	//     printf("DEBUG: AST args: ");
	//     while (ast->args[i])
	//     {
	//         printf("[%s] ", ast->args[i]);
	//         i++;
	//     }
	//     printf("\n");
	// }
	//DEBUG
	ignore_signals(); // El shell debe ignorar las señales mientras el AST se ejecuta.
	exit_status = execute_ast(ast, &heredoc_id, shell_context);
	setup_interactive_signals();
	cleanup_heredoc_files(shell_context);
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	return (exit_status);
}
