#include "../include/minishell.h"

int	main(int ac, char **av, char **envp)
{
	char			**envp_copy;
	int				exit_status;
	t_shell_context	*shell_context;
	(void)ac;
	(void)av;
	// 1. Reservar memoria para la estructura.
	shell_context = malloc(sizeof(t_shell_context));
	if (!shell_context)
		return (1);
	// 2. Inicializar el valor.
	shell_context->exit_status = 0;
	envp_copy = init_shell_environment(envp);
	// 3. Pasar el puntero a la estructura (no la dirección del puntero).
	exit_status = run_shell_loop(&envp_copy, shell_context);
	printf("exit status %i\n", exit_status);/*  */
	//(shell_context)->exit_status = exit_status;
	ft_freearr(envp_copy);
	rl_clear_history();
	// 4. Liberar la memoria al final.
	free(shell_context);
	return (exit_status);
}
