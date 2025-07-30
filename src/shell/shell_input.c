#include "../../include/minishell.h"

char	*get_user_input(void)
{
	char	*input;

	if (isatty(STDIN_FILENO))
	{
		input = readline(PROMPT);
		if (!input)
			return(NULL); //BORRAR Y DESCOMENTAR LA LINEA SIGUIENTE
	//		return (ft_putstr_fd("exit\n", STDOUT_FILENO), NULL);
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
// if (isatty(STDIN_FILENO)) => Modo Interactivo
// else => Modo No Interactivo
// Usamos GNL para leer del pipe sin imprimir nada
