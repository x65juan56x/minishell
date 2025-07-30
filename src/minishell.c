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
	printf("DEBUG: token list: ");
	while (cur)
	{
		printf("[%s] ", (char*)cur->value);
		cur = cur->next;
	}
	printf("\n");
}

void	debug_print_ast(const t_ast_node *node)
{
	printf("DEBUG: AST nodes:\n");
    if (!node)
    {
        printf("AST: (null)\n");
        return;
    }
    printf("AST Node: type=%d\n", node->type);
    if (node->args)
    {
        printf("  args: ");
        for (int i = 0; node->args[i]; i++)
            printf("[%s] ", node->args[i]);
        printf("\n");
    }
    if (node->file)
        printf("  file: [%s]\n", node->file);
    if (node->left)
    {
        printf("  left:\n");
        debug_print_ast(node->left);
    }
    if (node->right)
    {
        printf("  right:\n");
        debug_print_ast(node->right);
    }
}

void	debug_print_parser(const t_parser *parser)
{
    if (!parser)
    {
        printf("DEBUG: parser is NULL\n");
        return;
    }
    printf("DEBUG: t_parser\n");
    printf("  error: %d\n", parser->error);
    printf("  tokens: ");
    debug_print_token_list(parser->tokens);
    printf("  current token: ");
    if (parser->current)
        printf("[%s] (type=%d)\n", parser->current->value, parser->current->type);
    else
        printf("(null)\n");
}

void	debug_print_expand(t_token token)
{
	printf("DEBUG: expand: ");
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
