#include "../../include/minishell.h"

static void	child_process_routine(t_ast_node *node, t_pipe_config *config, int *heredoc_id_ptr, t_shell_context *shell_context)
{
	int	exit_code;

	setup_child_signals();
	if (config->is_left)
	{
		if (config->heredoc_fd != -1)
			dup2(config->heredoc_fd, STDIN_FILENO);
		dup2(config->pipe_fd[1], STDOUT_FILENO);
	}
	else
		dup2(config->pipe_fd[0], STDIN_FILENO);
	close(config->pipe_fd[0]);
	close(config->pipe_fd[1]);
	if (config->heredoc_fd != -1)
		close(config->heredoc_fd);
	exit_code = execute_ast(node, heredoc_id_ptr, shell_context);
	exit(exit_code);
}

pid_t	create_pipe_child(t_ast_node *node, t_pipe_config *config, int *heredoc_id_ptr, t_shell_context *shell_context)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (perror("fork"), -1);
	if (pid == 0)
		child_process_routine(node, config, heredoc_id_ptr, shell_context);
	return (pid);
}
