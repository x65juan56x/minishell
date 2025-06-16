#include "../include/minishell.h"

int	main(int ac, char **av, char **envp)
{
	char    *input;

	(void)ac;
	(void)av;
	while (1)
	{
		input = readline(PROMPT);
		if (!input)
		{
			printf("\n");
			break;
		}
		if (*input)
			add_history(input);
		if (ft_strcmp(input, "exit") == 0)
		{
			free(input);
			break;
		}
        t_token *tokens = tokenize(input);
        if (!tokens)
        {
            printf("Error: Failed to tokenize input\n");
            free(input);
            continue;
        }
        t_ast_node *ast = parse(tokens);
        if (!ast)
        {
            printf("Error: Failed to parse tokens\n");
            cleanup_tokens(tokens);
            free(input);
            continue;
        }
        printf("Executing command...\n");
        int exit_status = execute_ast(ast, envp);
        printf("Command finished with exit status: %d\n", exit_status);
        cleanup_ast(ast);
        cleanup_tokens(tokens);
		free(input);
	}
	rl_clear_history();
	return (0);
}
