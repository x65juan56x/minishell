#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens, t_ast_node *ast,
			t_shell_context *shell_context)
{
	if (tokens == NULL)
	{
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
		return (2);
	}
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

int	process_command_line(t_token *tokens, t_shell_context *shell_context)
{
	t_ast_node	*ast;
	int			exit_status;
	int			heredoc_id;

	heredoc_id = 0;
	shell_context->heredoc_files = NULL;

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
