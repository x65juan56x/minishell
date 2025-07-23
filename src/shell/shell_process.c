#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens, t_ast_node *ast)
{
	if (tokens->type != TOKEN_EOF)
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
	if (ast)
		cleanup_ast(ast);
	if (tokens)
		cleanup_tokens(tokens);
	return (2);
}

static int	pipes_checker(t_token *tokens)
{
	t_token	*last;

	last = tokens;
	while (last->next->next != NULL)
		last = last->next;
	if (tokens->type == 1 || tokens->type == 6 || last->type == 1)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);
		ft_putstr_fd(tokens->value, STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		return (1); // Debo liberar las listas antes de retornar?
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
	if (pipes_checker(tokens))
		return (cleanup_tokens(tokens), 2);
	expander_var(tokens, shell_context);
	tokens = expand_wildcards(tokens);
	ast = parse(tokens);
	if (!ast)
		return (cleanup_tokens(tokens), handle_parsing_error(tokens, ast));
	ignore_signals(); // El shell debe ignorar las señales mientras el AST se ejecuta.
	exit_status = execute_ast(ast, &heredoc_id, shell_context);
	setup_interactive_signals();
	cleanup_heredoc_files(shell_context);
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	return (exit_status);
}
