/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juan <juan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:25:24 by jmondon           #+#    #+#             */
/*   Updated: 2025/07/18 18:15:52 by juan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void*))
{
//	printf("DEBUG: ft_lstdelone llamada\n");/*DEBUG*/
	if (!lst)
		return ;
	del(lst->content);
	free(lst);
}
