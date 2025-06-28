#include "../../include/minishell.h"

static int	find_heredoc_in_node(t_ast_node *node)
{
	if (!node)
		return (-1);
	if (node->type == NODE_HEREDOC)
		return (execute_heredoc(node->file));
	return (-1);
}

static int	search_heredoc_recursive(t_ast_node *node)
{
	int	heredoc_fd;

	if (!node)
		return (-1);
	heredoc_fd = find_heredoc_in_node(node);
	if (heredoc_fd != -1)
		return (heredoc_fd);
	if (node->left)
	{
		heredoc_fd = search_heredoc_recursive(node->left);
		if (heredoc_fd != -1)
			return (heredoc_fd);
	}
	return (-1);
}

int	preprocess_heredocs(t_ast_node *node)
{
	return (search_heredoc_recursive(node));
}
