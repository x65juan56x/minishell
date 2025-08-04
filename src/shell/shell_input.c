#include "../../include/minishell.h"

char	*get_user_input(void)
{
	char	*input;

	if (isatty(STDIN_FILENO))
	{
		input = readline(CORAL_BOLD "MiniShell" RESET_COLOR " $ ");
		if (!input)
			return (/*ft_putstr_fd("exit\n", STDOUT_FILENO), */NULL);
	}
	else
		input = get_next_line(STDIN_FILENO);
	return (input);
}

int	handle_input_line(char *input)
{
	if (!input)
		return (0);
	if (*input)
		if (isatty(STDIN_FILENO))
			add_history(input);
	return (0);
}
