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
		signals();
		input = readline(PROMPT);
		if (!input)
		{
			printf("exit\n");
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
			free(input);
			continue;
		}
		ast = parse(tokens);
		if (!ast)
		{
			if (tokens->type != TOKEN_EOF)
                ft_putstr_fd("minishell: syntax error\n", 2);
			cleanup_tokens(tokens);
			free(input);
			continue;
		}
		exit_status = execute_ast(ast, envp);
		cleanup_ast(ast);
		cleanup_tokens(tokens);
		free(input);
		rl_on_new_line(); // Limpia el estado de readline
	}
	rl_clear_history();
	return (exit_status);
}
