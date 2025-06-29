#include "../../include/minishell.h"

static int	execute_command_node(t_ast_node *node, char **envp)
{
	if (!node || !node->args || !node->args[0])
		return (0);
	launch_command(node->args, envp);
	return (127);
}

static int	create_pipe_and_execute(t_ast_node *node, char **envp,
			int heredoc_fd)
{
	int				pipe_fd[2];
	t_pipe_config	left_config;
	t_pipe_config	right_config;
	pid_t			left_pid;
	pid_t			right_pid;

	if (pipe(pipe_fd) == -1)
	{
		if (heredoc_fd != -1)
			close(heredoc_fd);
		return (1);
	}
	left_config = (t_pipe_config){pipe_fd, 1, heredoc_fd, envp};
	right_config = (t_pipe_config){pipe_fd, 0, -1, envp};
	left_pid = create_pipe_child(node->left, &left_config);
	right_pid = create_pipe_child(node->right, &right_config);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	if (heredoc_fd != -1)
		close(heredoc_fd);
	return (wait_pipe_children(left_pid, right_pid));
}

static int	execute_pipe_node(t_ast_node *node, char **envp)
{
	int	heredoc_fd;

	heredoc_fd = preprocess_heredocs(node->left);
	return (create_pipe_and_execute(node, envp, heredoc_fd));
}

int	execute_ast(t_ast_node *ast, char **envp)
{
	pid_t	pid;
	int		status;

	if (!ast)
		return (0);
	if (ast->type == NODE_COMMAND)
	{
		pid = fork();
		if (pid == -1)
			return (perror("fork"), 1);
		if (pid == 0)
		{
			execute_command_node(ast, envp);
			exit(127);
		}
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		if (WIFSIGNALED(status))
			return (128 + WTERMSIG(status));
		return (1);
	}
	if (ast->type == NODE_PIPE)
		return (execute_pipe_node(ast, envp));
	if (is_redirect_node(ast->type))
		return (execute_redirect_node(ast, envp));
	return (1);
}
