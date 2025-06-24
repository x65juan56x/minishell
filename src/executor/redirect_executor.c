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
		fd = -1;
	if (fd == -1)
	{
		perror(file);
		return (-1);
	}
	return (fd);
}

static int	setup_redirect(t_ast_node *node)
{
	int	fd;
	int	target_fd;

	fd = open_redirect_file(node->file, node->type);
	if (fd == -1)
		return (1);
	if (node->type == NODE_REDIRECT_IN)
		target_fd = STDIN_FILENO;
	else
		target_fd = STDOUT_FILENO;
	if (dup2(fd, target_fd) == -1)
	{
		close(fd);
		perror("dup2");
		return (1);
	}
	close(fd);
	return (0);
}

int	execute_redirect_node(t_ast_node *node, char **envp)
{
	pid_t	pid;
	int		status;

	if (!node || !node->left || !node->file)
		return (1);
	pid = fork();
	if (pid == -1)
		return (1);
	if (pid == 0)
	{
		if (setup_redirect(node) != 0)
			exit(1);
		execute_ast(node->left, envp);
		exit(1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

