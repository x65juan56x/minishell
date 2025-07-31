#include "../../include/minishell.h"

int	count_pipe_commands(t_ast_node *ast)
{
	int			count;
	t_ast_node	*current;

	count = 0;
	current = ast;
	while (current)
	{
		count++;
		if (current->type != NODE_PIPE)
			break ;
		current = current->right;
	}
	return (count);
}

void	child_process_logic(t_pipe_state *st, int pipe_fd[2], int is_last,
		int *heredoc_id_ptr, t_shell_context *shell_context)
{
	free(st->pids);
	setup_child_signals();
	if (st->prev_pipe_fd != -1)
	{
		dup2(st->prev_pipe_fd, STDIN_FILENO);
		close(st->prev_pipe_fd);
	}
	if (!is_last)
	{
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		exit(execute_ast(st->curr->left, heredoc_id_ptr, shell_context));
	}
	else
		exit(execute_ast(st->curr, heredoc_id_ptr, shell_context));
}

int	parent_process_logic(t_pipe_state *st, int pipe_fd[2])
{
	if (st->prev_pipe_fd != -1)
		close(st->prev_pipe_fd);
	st->prev_pipe_fd = pipe_fd[0];
	close(pipe_fd[1]);
	st->curr = st->curr->right;
	st->i++;
	return (0);
}
