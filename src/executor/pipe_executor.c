#include "../../include/minishell.h"

static int	init_pipe_state(t_pipe_state *st, t_ast_node *ast, char ***envp_ptr)
{
	st->num_cmds = count_pipe_commands(ast);
	st->pids = malloc(sizeof(pid_t) * st->num_cmds);
	if (!st->pids)
		return (perror("minishell: malloc"), 1);
	st->i = 0;
	st->curr = ast;
	st->prev_pipe_fd = -1;
	st->envp_ptr = envp_ptr;
	return (0);
}

static int	execute_pipe_segment(t_pipe_state *st, int *heredoc_id_ptr)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (perror("minishell: pipe"), 1);
	st->pids[st->i] = fork();
	if (st->pids[st->i] == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (perror("minishell: fork"), 1);
	}
	if (st->pids[st->i] == 0)
		child_process_logic(st, pipe_fd, 0, heredoc_id_ptr);
	parent_process_logic(st, pipe_fd);
	return (0);
}

static int	execute_last_command(t_pipe_state *st, int *heredoc_id_ptr)
{
	st->pids[st->i] = fork();
	if (st->pids[st->i] == -1)
		return (perror("minishell: fork"), 1);
	if (st->pids[st->i] == 0)
		child_process_logic(st, NULL, 1, heredoc_id_ptr);
	if (st->prev_pipe_fd != -1)
		close(st->prev_pipe_fd);
	return (0);
}

int	execute_pipe_line(t_ast_node *ast, char ***envp_ptr, int *heredoc_id_ptr)
{
	t_pipe_state	st;

	if (init_pipe_state(&st, ast, envp_ptr) != 0)
		return (1);
	while (st.curr->type == NODE_PIPE)
	{
		if (execute_pipe_segment(&st, heredoc_id_ptr) != 0)
			return (free(st.pids), 1);
	}
	if (execute_last_command(&st, heredoc_id_ptr) != 0)
		return (free(st.pids), 1);
	return (wait_for_all_children(st.pids, st.num_cmds));
}
