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
	int			heredoc_id;

	heredoc_id = 0;
	tokens = tokenize(input);
	if (!tokens)
		return (1);
	expander_var(tokens);
	ast = parse(tokens);
	if (!ast)
	{
		exit_status = handle_parsing_error(tokens);
		return (cleanup_tokens(tokens), exit_status);
	}
	ignore_signals(); // El shell debe ignorar las señales mientras el AST se ejecuta.
	exit_status = execute_ast(ast, envp_ptr, &heredoc_id);
	setup_interactive_signals();
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	return (exit_status);
}
