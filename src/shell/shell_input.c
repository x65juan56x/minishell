#include "../../include/minishell.h"

char	*get_user_input(void)
{
	char	*input;

	if (isatty(STDIN_FILENO)) // Modo Interactivo
	{
		input = readline(PROMPT); // Usamos readline para tener el prompt y el historial
		if (!input) // Ctrl+D
			return (ft_putstr_fd("exit\n", STDOUT_FILENO), NULL);
	}
	else // Modo No Interactivo
		input = ft_mini_gnl_char(STDIN_FILENO); // Usamos ft_mini_gnl para leer del pipe sin imprimir nada
	return (input);
}

int	handle_input_line(char *input)
{
	if (!input)
		return (0);
	if (*input)
		if (isatty(STDIN_FILENO))
			add_history(input);
	if (ft_strcmp(input, "exit") == 0)
		return (1);
	return (0);
}
