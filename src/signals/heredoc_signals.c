/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_signals.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:20:33 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/06 17:23:36 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	heredoc_sigint_handler(int signum)
{
	(void)signum;
	exit(130);
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = heredoc_sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("minishell: sigaction");
	ft_bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("minishell: sigaction");
}
