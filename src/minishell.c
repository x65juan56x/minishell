#include "../include/minishell.h"

void	cleanup_shell_context(t_shell_context *shell_context)
{
	if (!shell_context)
		return ;
	if (shell_context->envp_cpy)
		ft_freearr(shell_context->envp_cpy);
	if (shell_context->local_vars)
		ft_lstclear(&shell_context->local_vars, free);
	cleanup_heredoc_files(shell_context);
	free(shell_context);
}

int	main(int ac, char **av, char **envp)
{
	int				exit_status;
	t_shell_context	*shell_context;

	(void)ac;
	(void)av;
	shell_context = malloc(sizeof(t_shell_context));
	if (!shell_context)
		return (1);
	shell_context->exit_status = 0;
	shell_context->should_exit = 0;
	shell_context->heredoc_files = NULL;
	shell_context->local_vars = NULL;
	shell_context->envp_cpy = init_shell_environment(envp, shell_context);
	update_shell_level(shell_context);
	shell_context->error_flag = 0;
	exit_status = run_shell_loop(shell_context);
	rl_clear_history();
	cleanup_shell_context(shell_context);
	return (exit_status);
}
