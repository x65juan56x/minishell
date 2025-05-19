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

#include "libft.h"

static int	count_args(const char *s)
{
    int count = 0, i = 0, in_arg = 0;
    while (s[i])
    {
        while (s[i] == ' ')
            i++;
        if (!s[i])
            break;
        in_arg = 1;
        while (s[i] && (in_arg))
        {
            if (s[i] == '\'' || s[i] == '"')
            {
                char quote = s[i++];
                while (s[i] && s[i] != quote)
                    i++;
                if (s[i] == quote)
                    i++;
            }
            else if (s[i] == ' ')
                in_arg = 0;
            else
                i++;
        }
        count++;
    }
    return count;
}

static char	*extract_arg(const char *s, int *i)
{
    int		start = *i, len = 0;
    char	*res = NULL;

    while (s[*i] == ' ')
        (*i)++;
    start = *i;
    while (s[*i] && s[*i] != ' ')
    {
        if (s[*i] == '\'' || s[*i] == '"')
        {
            char quote = s[(*i)++];
            while (s[*i] && s[*i] != quote)
                (*i)++;
            if (s[*i] == quote)
                (*i)++;
        }
        else
            (*i)++;
    }
    len = *i - start;
    res = (char *)malloc(len + 1);
    if (!res)
        return NULL;
    int j = 0, k = start;
    while (k < *i)
    {
        if (s[k] == '\'' || s[k] == '"')
        {
            char quote = s[k++];
            while (s[k] && s[k] != quote)
                res[j++] = s[k++];
            if (s[k] == quote)
                k++;
        }
        else
            res[j++] = s[k++];
    }
    res[j] = '\0';
    return res;
}

char	**ft_split_args(const char *s)
{
    int		i = 0, argc, idx = 0;
    char	**args;

    if (!s)
        return NULL;
    argc = count_args(s);
    args = malloc(sizeof(char *) * (argc + 1));
    if (!args)
        return NULL;
    while (idx < argc)
    {
        args[idx] = extract_arg(s, &i);
        if (!args[idx])
            return ft_freearr(args);
        idx++;
    }
    args[idx] = NULL;
    return args;
}
