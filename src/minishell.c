#include "../include/minishell.h"

static char	**copy_envp(char **envp)
{
	char	**new_envp;
	int		i;
	int		count;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = -1;
	while (++i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (ft_freearr(new_envp), NULL);
	}
	new_envp[i] = NULL;
	return (new_envp);
}

int	main(int ac, char **av, char **envp)
{
	char    	*input;
	t_token		*tokens;
	t_ast_node	*ast;
	int			exit_status;
	char		**envp_copy;

	(void)ac;
	(void)av;
	envp_copy = copy_envp(envp);
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
		exit_status = execute_ast(ast, &envp_copy);
		cleanup_ast(ast);
		cleanup_tokens(tokens);
		free(input);
		rl_on_new_line(); // Limpia el estado de readline
	}
	rl_clear_history();
	return (exit_status);
}
