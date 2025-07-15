#include "../include/minishell.h"

// Función de limpieza para el contexto
static void	cleanup_shell_context(t_shell_context *shell_context)
{
    if (!shell_context)
        return;
    if (shell_context->envp_cpy)
        ft_freearr(shell_context->envp_cpy);
    cleanup_heredoc_files(shell_context); // La limpieza de heredoc_files ya se hace en el bucle, pero por si acaso.
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
	shell_context->heredoc_files = NULL;
	shell_context->envp_cpy = init_shell_environment(envp);
	exit_status = run_shell_loop(shell_context);
	printf("exit status %i\n", exit_status);
	rl_clear_history();
	cleanup_shell_context(shell_context);
	return (exit_status);
}
