#include "../../include/minishell.h"

static int	handle_parsing_error(t_token *tokens)
{
	if (tokens->type != TOKEN_EOF)
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
	return (2);
}
void print_token_list(t_token *head)
{
    t_token *current = head;
    int token_count = 0;

    printf("--- Lista de Tokens ---\n");
    if (current == NULL)
    {
        printf("La lista está vacía.\n");
        printf("-----------------------\n");
        return;
    }

    while (current != NULL)
    {
        printf("Token %d:\n", token_count);
        printf("  Value: \"%s\"\n", current->value ? current->value : "(NULL)");
        printf("  Expand: %d\n", current->expand);
        current = current->next;
        token_count++;
    }
    printf("-----------------------\n");
}
int	process_command_line(char *input, char ***envp_ptr)
{
	t_token		*tokens;
	t_ast_node	*ast;
	int			exit_status;
	char *resultado_prueba;
	tokens = tokenize(input);
	if (!tokens)
		return (0);
	resultado_prueba = expander_var(tokens);
	printf("%s\n", resultado_prueba);
	ast = parse(tokens);
	if (!ast)
	{
		exit_status = handle_parsing_error(tokens);
		cleanup_tokens(tokens);
		return (exit_status);
	}
	exit_status = execute_ast(ast, envp_ptr);
	cleanup_ast(ast);
	cleanup_tokens(tokens);
	rl_on_new_line();
	return (exit_status);
}
