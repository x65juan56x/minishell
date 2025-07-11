#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens)
{
	if (tokens->type != TOKEN_EOF)
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
	return (2);
}

int	process_command_line(char *input, char ***envp_ptr)
{
    t_token		*tokens;
    t_ast_node	*ast;
    int			exit_status;

    tokens = tokenize(input);
    if (!tokens)
        return (0);
    expander_var(tokens);
    ast = parse(tokens);
    if (!ast)
    {
        exit_status = handle_parsing_error(tokens);
        cleanup_tokens(tokens);
        return (exit_status);
    }
    // El shell debe ignorar las señales mientras el AST se ejecuta.
    ignore_signals();
    exit_status = execute_ast(ast, envp_ptr);
    cleanup_ast(ast);
    cleanup_tokens(tokens);
    // No es necesario rl_on_new_line() aquí, se maneja en el handler.
    return (exit_status);
}
