/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:21:06 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:21:07 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

static char	**create_sorted_array(char **envp_cpy, int count)
{
	char	**sorted_envp;
	int		i;

	sorted_envp = malloc(sizeof(char *) * (count + 1));
	if (!sorted_envp)
		return (NULL);
	i = -1;
	while (++i < count)
		sorted_envp[i] = envp_cpy[i];
	sorted_envp[count] = NULL;
	return (sorted_envp);
}

static void	bubble_sort_array(char **sorted_envp, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = -1;
	while (++i < count - 1)
	{
		j = -1;
		while (++j < count - 1 - i)
		{
			if (ft_strcmp(sorted_envp[j], sorted_envp[j + 1]) > 0)
			{
				temp = sorted_envp[j];
				sorted_envp[j] = sorted_envp[j + 1];
				sorted_envp[j + 1] = temp;
			}
		}
	}
}

static void	print_export_var(char *env_var)
{
	char	*temp;
	char	*name;
	char	*value;

	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	temp = ft_strchr(env_var, '=');
	if (temp)
	{
		name = ft_substr(env_var, 0, temp - env_var);
		value = temp + 1;
		ft_putstr_fd(name, STDOUT_FILENO);
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(value, STDOUT_FILENO);
		ft_putendl_fd("\"", STDOUT_FILENO);
		free(name);
	}
}

static void	print_sorted_export(char **sorted_envp, int count)
{
	int	i;

	i = -1;
	while (++i < count)
		print_export_var(sorted_envp[i]);
}

void	sort_and_print_export(char **envp_cpy)
{
	char	**sorted_envp;
	int		count;

	count = 0;
	while (envp_cpy[count])
		count++;
	sorted_envp = create_sorted_array(envp_cpy, count);
	if (!sorted_envp)
		return ;
	bubble_sort_array(sorted_envp, count);
	print_sorted_export(sorted_envp, count);
	free(sorted_envp);
}
