#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens)
{
	if (tokens->type != TOKEN_EOF)
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
	return (2);
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
//	printf("En process_command_line antes de expander_var\n");/*DEBUG*/
//	debug_print_local_vars(shell_context);/*DEBUG*/
	expander_var(tokens, shell_context);
//	printf("En process_command_line después de expander_var\n");/*DEBUG*/
//	debug_print_local_vars(shell_context);/*DEBUG*/
	tokens = expand_wildcards(tokens);
	ast = parse(tokens);
	if (!ast)
	{
		exit_status = handle_parsing_error(tokens);
		return (cleanup_tokens(tokens), exit_status);
	}
//	printf("En process_command_line después de parse\n");/*DEBUG*/
//	debug_print_local_vars(shell_context);/*DEBUG*/
	ignore_signals(); // El shell debe ignorar las señales mientras el AST se ejecuta.
//	printf("En process_command_line antes de execute_ast\n");/*DEBUG*/
//	debug_print_local_vars(shell_context);/*DEBUG*/
	exit_status = execute_ast(ast, &heredoc_id, shell_context);
//	printf("En process_command_line después de execute_ast\n");/*DEBUG*/
//	debug_print_local_vars(shell_context);/*DEBUG*/
	setup_interactive_signals();
	cleanup_heredoc_files(shell_context);
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	return (exit_status);
}
