#include "../../include/minishell.h"

char	*get_user_input(void)
{
	char	*input;
	char	*prompt;

	if (isatty(STDIN_FILENO))
		prompt = PROMPT;
	else
		prompt = "";
	input = readline(prompt);
	if (!input)
	{
		if (isatty(STDIN_FILENO))
			ft_putstr_fd("exit\n", STDOUT_FILENO);
		return (NULL);
	}
	return (input);
}

int	handle_input_line(char *input)
{
	if (!input)
		return (0);
	if (*input)
	{
		if (isatty(STDIN_FILENO))
			add_history(input);
	}
	if (ft_strcmp(input, "exit") == 0)
		return (1);
	return (0);
}
