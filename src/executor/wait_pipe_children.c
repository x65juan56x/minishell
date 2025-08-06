/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_pipe_children.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:22:51 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:37:15 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	get_status_from_child(int status, int *signal_num)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		*signal_num = WTERMSIG(status);
		return (128 + *signal_num);
	}
	return (status);
}

int	wait_for_all_children(pid_t *pids, int num_cmds)
{
	int	i;
	int	status;
	int	final_status;
	int	signal_num;

	i = 0;
	status = 0;
	final_status = 0;
	signal_num = 0;
	while (i < num_cmds)
	{
		waitpid(pids[i], &status, 0);
		if (i == num_cmds - 1)
			final_status = get_status_from_child(status, &signal_num);
		i++;
	}
	free(pids);
	if (signal_num != 0)
		print_signal_message(signal_num);
	return (final_status);
}
