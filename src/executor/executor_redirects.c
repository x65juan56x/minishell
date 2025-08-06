/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirects.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:05 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:36:55 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	open_redirect_file(char *file, t_node_type type)
{
	int	fd;

	if (type == NODE_REDIRECT_IN)
		fd = open(file, O_RDONLY);
	else if (type == NODE_REDIRECT_OUT)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (type == NODE_REDIRECT_APPEND)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return (-1);
	if (fd < 0)
		perror(file);
	return (fd);
}

int	apply_redirections(t_ast_node *node)
{
	int	fd;
	int	target_fd;

	if (!node || !is_redirect_node(node->type))
		return (0);
	if (node->type == NODE_HEREDOC)
		fd = open(node->file, O_RDONLY);
	else
		fd = open_redirect_file(node->file, node->type);
	if (fd < 0)
		return (1);
	if (node->type == NODE_REDIRECT_IN || node->type == NODE_HEREDOC)
		target_fd = STDIN_FILENO;
	else
		target_fd = STDOUT_FILENO;
	if (dup2(fd, target_fd) == -1)
		return (perror("minishell: dup2"), close(fd), 1);
	close(fd);
	return (0);
}
