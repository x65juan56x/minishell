/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:20:38 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:34 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens, t_ast_node *ast)
{
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
	ast = parse(tokens, shell_context);
	if (!ast)
		return (handle_parsing_error(tokens, ast));
	shell_context->current_ast = ast;
	shell_context->current_tokens = tokens;
	ignore_signals();
	exit_status = execute_ast(ast, &heredoc_id, shell_context);
	setup_interactive_signals();
	shell_context->current_ast = NULL;
	shell_context->current_tokens = NULL;
	cleanup_heredoc_files(shell_context);
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	return (exit_status);
}
