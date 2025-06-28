/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:30:59 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/12 18:01:01 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static void	skip_one_arg_for_count(const char *s, int *idx)
{
	char	quote_char;

	if (s[*idx] == '\'' || s[*idx] == '"')
	{
		quote_char = s[(*idx)++];
		while (s[*idx] && s[*idx] != quote_char)
			(*idx)++;
		if (s[*idx])
			(*idx)++;
	}
	else
	{
		while (s[*idx] && s[*idx] != ' ' && s[*idx] != '\'' && s[*idx] != '"')
			(*idx)++;
	}
}

static int	ft_count_args(const char *s)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] == ' ')
			i++;
		if (s[i] == '\0')
			break ;
		count++;
		skip_one_arg_for_count(s, &i);
	}
	return (count);
}

static void	get_arg_details(const char *s, int *s_idx,
				int *c_start_out, int *c_len_out)
{
	char	quote_char;
	int		current_pos;

	current_pos = *s_idx;
	if (s[current_pos] == '\'' || s[current_pos] == '"')
	{
		quote_char = s[current_pos];
		current_pos++;
		*c_start_out = current_pos;
		while (s[current_pos] && s[current_pos] != quote_char)
			current_pos++;
		*c_len_out = current_pos - *c_start_out;
		if (s[current_pos] == quote_char)
			current_pos++;
	}
	else
	{
		*c_start_out = current_pos;
		while (s[current_pos] && s[current_pos] != ' '
			&& s[current_pos] != '\'' && s[current_pos] != '"')
			current_pos++;
		*c_len_out = current_pos - *c_start_out;
	}
	*s_idx = current_pos;
}

static int	get_one_arg(const char *s, int *s_pos, char **arg_dest)
{
	int	content_s;
	int	content_l;
	int	current_pos;

	current_pos = *s_pos;
	while (s[current_pos] == ' ')
		current_pos++;
	if (s[current_pos] == '\0')
		return (0);
	get_arg_details(s, &current_pos, &content_s, &content_l);
	*arg_dest = ft_substr(s, content_s, content_l);
	if (!*arg_dest)
		return (-1);
	*s_pos = current_pos;
	return (1);
}

char	**ft_split_args(char const *s)
{
	char	**result_array;
	int		num_args;
	int		i;
	int		current_s_pos;

	if (!s)
		return (NULL);
	num_args = ft_count_args(s);
	result_array = (char **)malloc((num_args + 1) * sizeof(char *));
	if (!result_array)
		return (NULL);
	i = 0;
	current_s_pos = 0;
	while (i < num_args)
	{
		if (get_one_arg(s, &current_s_pos, &result_array[i]) != 1)
			return (ft_freearr(result_array));
		i++;
	}
	result_array[i] = NULL;
	return (result_array);
}
