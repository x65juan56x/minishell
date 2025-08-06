/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:33 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:37:06 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	print_warning_hd(void)
{
	if (isatty(STDIN_FILENO))
	{
		ft_putstr_fd("minishell: warning: here-docu", STDERR_FILENO);
		ft_putstr_fd("ment delimited by end-of-file\n", STDERR_FILENO);
	}
}

int	read_heredoc_input(char *delimiter, int write_fd,
		t_shell_context *shell_context)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			line = readline("> ");
		else
			line = get_next_line(STDIN_FILENO);
		if (!line)
			return (print_warning_hd(), 1);
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		expanded_line = expander_line_content(line, shell_context);
		ft_putendl_fd(expanded_line, write_fd);
		free(line);
		free(expanded_line);
	}
	return (0);
}

int	create_heredoc_file(const char *filename, char *delimiter,
		t_shell_context *shell_context)
{
	int	fd;

	fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0600);
	if (fd < 0)
		return (perror("minishell: heredoc"), -1);
	read_heredoc_input(delimiter, fd, shell_context);
	close(fd);
	return (0);
}
