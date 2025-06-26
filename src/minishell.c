#include "../include/minishell.h"

int	main(int ac, char **av, char **envp)
{
	char    	*input;
	t_token		*tokens;
	t_ast_node	*ast;
	int			exit_status;

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
		tokens = tokenize(input);
		if (!tokens)
		{
			ft_putstr_fd("Error: Failed to tokenize input\n", 2);
			free(input);
			continue;
		}
		ast = parse(tokens);
		if (!ast)
		{
			ft_putstr_fd("Error: Failed to parse tokens\n", 2);
			cleanup_tokens(tokens);
			free(input);
			continue;
		}
		exit_status = execute_ast(ast, envp);
		cleanup_ast(ast);
		cleanup_tokens(tokens);
		free(input);
	}
	rl_clear_history();
	return (exit_status);
}
