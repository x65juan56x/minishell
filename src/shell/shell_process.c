#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens, t_ast_node *ast,
			t_shell_context *shell_context)
{
	(void)shell_context; // Ya no es necesario
	// El mensaje de error ya ha sido impreso por el parser.
	// Solo necesitamos limpiar y devolver el código de estado.
	if (ast)
		cleanup_ast(ast);
	if (tokens)
		cleanup_tokens(tokens);
	return (2); // Código de error para error de sintaxis
}

int	process_command_line(t_token *tokens, t_shell_context *shell_context)
{
	t_ast_node	*ast;
	int			exit_status;
	int			heredoc_id;

	heredoc_id = 0;
	shell_context->heredoc_files = NULL;

	expander_var(tokens, shell_context);
	tokens = expand_wildcards(tokens);

	ast = parse(tokens, shell_context);
	if (!ast)
		return (handle_parsing_error(tokens, ast, shell_context));

	ignore_signals(); // El shell debe ignorar las señales mientras el AST se ejecuta.
	exit_status = execute_ast(ast, &heredoc_id, shell_context);
	setup_interactive_signals();
	cleanup_heredoc_files(shell_context);
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	return (exit_status);
}
