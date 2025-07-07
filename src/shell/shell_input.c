#include "../../include/minishell.h"

char	*get_user_input(void)
{
	char	*input;

	signals_parent();
	input = readline(PROMPT);
	if (!input)
		return (printf("exit\n"), NULL);
	return (input);
}

int	handle_input_line(char *input)
{
	if (!*input)
		return (0);
	add_history(input);
	if (ft_strcmp(input, "exit") == 0)
		return (1);
	return (0);
}
