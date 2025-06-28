#include "../../include/minishell.h"

static void	setup_pipe_child(int *pipe_fd, int is_left_child)
{
	if (is_left_child)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			close(pipe_fd[1]);
			exit(1);
		}
		close(pipe_fd[1]);
	}
	else
	{
		close(pipe_fd[1]);
		if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
		{
			close(pipe_fd[0]);
			exit(1);
		}
		close(pipe_fd[0]);
	}
}

static void	setup_heredoc_input(int heredoc_fd)
{
	if (heredoc_fd == -1)
		return ;
	if (dup2(heredoc_fd, STDIN_FILENO) == -1)
		exit(1);
	close(heredoc_fd);
}

static void	child_process_routine(t_ast_node *node, char **envp)
{
	if (node->type == NODE_COMMAND)
	{
		if (node->args && node->args[0])
			launch_command(node->args, envp);
	}
	else
		execute_ast(node, envp);
	exit(1);
}

pid_t	create_pipe_child(t_ast_node *node, t_pipe_config *config)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		if (config->is_left)
			close(config->pipe_fd[1]);
		else
			close(config->pipe_fd[0]);
		if (config->heredoc_fd != -1)
			close(config->heredoc_fd);
		return (-1);
	}
	if (pid == 0)
	{
		if (config->is_left)
			setup_heredoc_input(config->heredoc_fd);
		setup_pipe_child(config->pipe_fd, config->is_left);
		child_process_routine(node, config->envp);
	}
	return (pid);
}

int	wait_pipe_children(pid_t left_pid, pid_t right_pid)
{
	int	left_status;
	int	right_status;

	if (left_pid > 0)
		waitpid(left_pid, &left_status, 0);
	if (right_pid > 0)
		waitpid(right_pid, &right_status, 0);
	if (right_pid > 0 && WIFEXITED(right_status))
		return (WEXITSTATUS(right_status));
	if (right_pid > 0 && WIFSIGNALED(right_status))
		return (128 + WTERMSIG(right_status));
	if (left_pid > 0 && WIFEXITED(left_status))
		return (WEXITSTATUS(left_status));
	if (left_pid > 0 && WIFSIGNALED(left_status))
		return (128 + WTERMSIG(left_status));
	return (1);
}
