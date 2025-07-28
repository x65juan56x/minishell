#include "../include/minishell.h"

void	debug_print_local_vars(t_shell_context *ctx)
{
	t_list *cur = ctx->local_vars;
	printf("DEBUG: local_vars: ");
	while (cur)
	{
		printf("[%s] ", (char*)cur->content);
		cur = cur->next;
	}
	printf("\n");
}

void	debug_print_token_list(t_token *head)
{
	t_token *cur = head;
	//printf("DEBUG: token list: ");
	while (cur)
	{
		printf("[%s] ", (char*)cur->value);
		cur = cur->next;
	}
	printf("\n");
}

void	debug_print_expand(t_token token)
{
	//printf("DEBUG: expand: ");
	printf("[%i] ", token.expand);
	printf("\n");
}

// Función de limpieza para el contexto
void	cleanup_shell_context(t_shell_context *shell_context)
{
	if (!shell_context)
		return;
	if (shell_context->envp_cpy)
		ft_freearr(shell_context->envp_cpy);
	if (shell_context->local_vars) // Limpiar variables locales
		ft_lstclear(&shell_context->local_vars, free);
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
	shell_context->local_vars = NULL;
	shell_context->envp_cpy = init_shell_environment(envp, shell_context);
	shell_context->error_flag = 0;
	exit_status = run_shell_loop(shell_context);
	rl_clear_history();
	cleanup_shell_context(shell_context);
	return (exit_status);
}
