#include "../../include/minishell.h"

static int	create_heredoc_pipe(int *pipe_fd)
{
	if (pipe(pipe_fd) == -1)
	{
		perror("minishell: pipe");
		return (1);
	}
	return (0);
}

static void	write_line_to_pipe(int write_fd, char *line)
{
	if (!line)
		return ;
	write(write_fd, line, ft_strlen(line));
	write(write_fd, "\n", 1);
}

static int	read_heredoc_input(char *delimiter, int write_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		write_line_to_pipe(write_fd, line);
		free(line);
	}
	return (0);
}

int	execute_heredoc(char *delimiter)
{
	int	pipe_fd[2];

	if (create_heredoc_pipe(pipe_fd) != 0)
		return (-1);
	read_heredoc_input(delimiter, pipe_fd[1]);
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}
