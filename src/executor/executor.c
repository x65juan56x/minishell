#include "../../include/minishell.h"

static int	handle_simple_command_fork(t_ast_node *ast, char ***envp_ptr, t_shell_context *shell_context)
{
	pid_t	pid;
	int		status;
	int		exit_code;

	if (ast->type == NODE_COMMAND && is_builtin_parent(ast->args[0]))
		return (execute_builtin(ast->args, envp_ptr));
	ignore_signals();
	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		setup_child_signals();
		exit(execute_simple_command(ast, envp_ptr));
	}
	waitpid(pid, &status, 0);
	setup_interactive_signals();
	exit_code = (analyze_child_status(status));
	shell_context->exit_status = exit_code;
	return (exit_code);
}

static int	handle_logical_op(t_ast_node *ast, char ***envp_ptr, int *heredoc_id_ptr, t_shell_context *shell_context)
{
	int	status;

	status = execute_ast(ast->left, envp_ptr, heredoc_id_ptr, shell_context);
	if (ast->type == NODE_OR && status == 0)
		return (0);
	if (ast->type == NODE_AND && status != 0)
		return (status);
	return (execute_ast(ast->right, envp_ptr, heredoc_id_ptr, shell_context));
}

int	execute_ast(t_ast_node *ast, char ***envp_ptr, int *heredoc_id_ptr, t_shell_context *shell_context)
{
	if (!ast)
		return (0);
	if (preprocess_heredocs(&ast, heredoc_id_ptr, shell_context) != 0)
		return (130);
	if (ast->type == NODE_OR || ast->type == NODE_AND)
		return (handle_logical_op(ast, envp_ptr, heredoc_id_ptr, shell_context));
	if (ast->type == NODE_PIPE)
		return (execute_pipe_line(ast, envp_ptr, heredoc_id_ptr, shell_context));
	return (handle_simple_command_fork(ast, envp_ptr, shell_context));
}
