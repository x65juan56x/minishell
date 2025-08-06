/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:08 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:27 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	cleanup_child_process(t_shell_context *shell_context)
{
	int	count;

	if (!shell_context)
		return ;
	if (shell_context->envp_cpy)
	{
		count = 0;
		while (shell_context->envp_cpy[count])
			count++;
		ft_freearr(shell_context->envp_cpy);
		shell_context->envp_cpy = NULL;
	}
	if (shell_context->local_vars)
	{
		count = ft_lstsize(shell_context->local_vars);
		ft_lstclear(&shell_context->local_vars, free);
		shell_context->local_vars = NULL;
	}
	if (shell_context->heredoc_files)
	{
		count = ft_lstsize(shell_context->heredoc_files);
		cleanup_heredoc_files(shell_context);
	}
}

void	cleanup_child_process_basic(t_shell_context *shell_context)
{
	cleanup_child_process(shell_context);
	clear_history();
	force_global_cleanup();
}

void	cleanup_child_process_deep(t_shell_context *shell_context)
{
	if (shell_context && shell_context->current_ast)
	{
		cleanup_ast(shell_context->current_ast);
		shell_context->current_ast = NULL;
	}
	if (shell_context && shell_context->current_tokens)
	{
		cleanup_tokens(shell_context->current_tokens);
		shell_context->current_tokens = NULL;
	}
	cleanup_child_process(shell_context);
	clear_history();
	force_global_cleanup();
	if (shell_context)
		free(shell_context);
}

void	cleanup_child_process_with_parsing(t_shell_context *shell_context,
		t_ast_node *ast, t_token *tokens)
{
	cleanup_child_process_deep(shell_context);
	if (ast)
		cleanup_ast(ast);
	if (tokens)
		cleanup_tokens(tokens);
}

void	force_global_cleanup(void)
{
	rl_clear_history();
	clear_history();
	rl_cleanup_after_signal();
}
