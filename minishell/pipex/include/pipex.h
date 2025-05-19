/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:34 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/14 13:28:57 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/wait.h>
# include <fcntl.h>

/*===LIBFT====================================================================*/
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strchr(const char *s, int c);
char	**ft_split(char const *s, char c);
void	ft_putstr_fd(char *s, int fd);
void	*ft_freearr(char **arr);
void	*ft_calloc(size_t nmemb, size_t size);
size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *s);
void	ft_bzero(void *s, size_t n);
char	*ft_substr(char const *s, unsigned int start, size_t len);
void	*ft_memset(void *s, int c, size_t n);
void	ft_error(void);
void	ft_perror_exit(const char *prefix, int exit_code);
void	ft_cmd_not_found_exit(const char *cmd_name);
void	ft_permission_denied_exit(const char *cmd_name);
char	**ft_split_args(char const *s);
int		ft_mini_gnl(char **line);

/*===CHILDREN-STUFF===========================================================*/
pid_t	create_child_one(char **av, int *fd_pipe, char **envp);
pid_t	create_child_two(char **av, int *fd_pipe, char **envp,
			pid_t pid1);

/*===FIND-THE-PATH============================================================*/
char	*get_path(char *cmd, char **envp);

/*===CMD-RUNNER===============================================================*/
void	run_cmd(char *cmd, char **envp);

/*===PIPEX-UTILS-BONUS========================================================*/
int		open_mode(char *argv, int i);
void	wrong_args(void);

#endif
