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

int	get_exit_status_from_args(t_token *args_token)
{
	if (!args_token || args_token->type == TOKEN_EOF)
		return (0);
	if (!ft_isnumstr(args_token->value))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args_token->value, STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		return (2);
	}
	if (args_token->next && args_token->next->type != TOKEN_EOF)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	return ((unsigned char)ft_atoi(args_token->value));
}
