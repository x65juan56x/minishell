/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_multiline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:05 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:28 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	print_ueof_message(char **input_ptr)
{
	ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
	free(*input_ptr);
	*input_ptr = ft_strdup("");
}

static int	append_input_line(char **input_ptr, char *new_line)
{
	char	*temp;
	char	*new_input;

	temp = ft_strjoin(*input_ptr, " ");
	new_input = ft_strjoin(temp, new_line);
	free(temp);
	free(new_line);
	free(*input_ptr);
	*input_ptr = new_input;
	if (!*input_ptr)
		return (1);
	return (0);
}

static int	read_non_interactive_multiline(char **input_ptr)
{
	char	*full_input;

	if (should_handle_logical_ops(*input_ptr))
	{
		full_input = get_next_line(STDIN_FILENO);
		if (!full_input)
			return (print_ueof_message(input_ptr), 0);
		if (append_input_line(input_ptr, full_input))
			return (1);
		return (3);
	}
	return (print_ueof_message(input_ptr), 0);
}

static int	read_interactive_multiline(char **input_ptr)
{
	char	*full_input;

	g_signal_status = 0;
	full_input = readline("> ");
	if (g_signal_status == SIGINT)
		return (cancel_multiline_input(input_ptr), 2);
	if (!full_input)
		return (print_ueof_message(input_ptr), 0);
	if (append_input_line(input_ptr, full_input))
		return (1);
	return (3);
}

int	handle_multiline_input(char **input_ptr)
{
	int	status;

	while (needs_multiline(*input_ptr))
	{
		if (!isatty(STDIN_FILENO))
			status = read_non_interactive_multiline(input_ptr);
		else
			status = read_interactive_multiline(input_ptr);
		if (status != 3)
			return (status);
	}
	return (0);
}
