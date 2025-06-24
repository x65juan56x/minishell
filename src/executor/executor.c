#include "../../include/minishell.h"

static int	execute_command_node(t_ast_node *node, char **envp)
{
	pid_t	pid;
	int		status;

	if (!node || !node->args || !node->args[0])
		return (1);
	pid = fork();
	if (pid == -1)
		return (1);
	if (pid == 0)
	{
		run_cmd_from_args(node->args, envp);
		exit(127);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

static int	execute_pipe_node(t_ast_node *node, char **envp)
{
	int		pipe_fd[2];
	pid_t	left_pid;
	pid_t	right_pid;

	if (pipe(pipe_fd) == -1)
		return (1);
	left_pid = create_pipe_child(node->left, pipe_fd, 1, envp);
	right_pid = create_pipe_child(node->right, pipe_fd, 0, envp);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	return (wait_pipe_children(left_pid, right_pid));
}

int	execute_ast(t_ast_node *ast, char **envp)
{
	if (!ast)
		return (0);
	if (ast->type == NODE_COMMAND)
		return (execute_command_node(ast, envp));
	if (ast->type == NODE_PIPE)
		return (execute_pipe_node(ast, envp));
	if (ast->type == NODE_REDIRECT_OUT || ast->type == NODE_REDIRECT_IN
		|| ast->type == NODE_REDIRECT_APPEND || ast->type == NODE_HEREDOC)
		return (execute_redirect_node(ast, envp));
	return (1);
}


