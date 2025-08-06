/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_preprocessor.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:29 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:22:30 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

static char	*build_heredoc_filename(int heredoc_id)
{
	char	*filename;
	char	*pid_str;
	char	*id_str;
	char	*temp;

	pid_str = ft_itoa(ft_getpid());
	id_str = ft_itoa(heredoc_id);
	temp = ft_strjoin("/tmp/minishell-", pid_str);
	free(pid_str);
	if (!temp)
		return (free(id_str), NULL);
	filename = ft_strjoin(temp, id_str);
	free(temp);
	free(id_str);
	return (filename);
}

static int	process_heredoc_node(t_ast_node *node, int *heredoc_id_ptr,
		t_shell_context *shell_context)
{
	char	*temp_filename;
	t_list	*new_list_node;

	temp_filename = build_heredoc_filename(*heredoc_id_ptr);
	if (!temp_filename)
		return (-1);
	if (execute_heredoc(temp_filename, node->file, shell_context) != 0)
		return (free(temp_filename), -1);
	new_list_node = ft_lstnew(ft_strdup(temp_filename));
	if (!new_list_node || !new_list_node->content)
		return (free(temp_filename), -1);
	ft_lstadd_back(&shell_context->heredoc_files, new_list_node);
	(*heredoc_id_ptr)++;
	free(node->file);
	node->file = temp_filename;
	node->type = NODE_REDIRECT_IN;
	return (0);
}

int	preprocess_heredocs(t_ast_node **node_ptr, int *hd_id_ptr,
		t_shell_context *shell_context)
{
	t_ast_node	*node;

	if (!node_ptr || !*node_ptr)
		return (0);
	node = *node_ptr;
	if (node->left)
		if (preprocess_heredocs(&(node->left), hd_id_ptr, shell_context) != 0)
			return (-1);
	if (node->right)
		if (preprocess_heredocs(&(node->right), hd_id_ptr, shell_context) != 0)
			return (-1);
	if (node->type == NODE_HEREDOC)
		return (process_heredoc_node(node, hd_id_ptr, shell_context));
	return (0);
}
