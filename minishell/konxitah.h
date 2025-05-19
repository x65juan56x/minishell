#ifndef KONXITAH_H
# define  KONXITAH_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include "libs/libft/libft.h"

/* COMAND RUNNER*/

void	run_cmd(char *cmd_str, char **envp);
char	*get_path(char *cmd, char **envp);

#endif