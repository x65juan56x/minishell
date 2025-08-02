#include "../../include/minishell.h"

t_ast_node	*collect_redirects(t_ast_node *node, t_list **redirect_list)
{
	t_ast_node	*current_node;
	t_list		*new_list_node;

	current_node = node;
	*redirect_list = NULL;
	while (current_node && is_redirect_node(current_node->type))
	{
		new_list_node = ft_lstnew(current_node);
		if (!new_list_node)
		{
			ft_lstclear(redirect_list, NULL);
			return (NULL);
		}
		ft_lstadd_front(redirect_list, new_list_node);
		current_node = current_node->left;
	}
	return (current_node);
}

int	apply_redirect_list(t_list *redirect_list)
{
	t_list	*current;

	current = redirect_list;
	while (current)
	{
		if (apply_redirections((t_ast_node *)current->content) != 0)
			return (1);
		current = current->next;
	}
	return (0);
}

int	handle_variable_assignment(char **args, t_shell_context *context)
{
	char	*eq_ptr;
	char	*var_name;
	t_list	*new_node;

	eq_ptr = ft_strchr(args[0], '=');
	if (!eq_ptr || eq_ptr == args[0])
		return (0);
	var_name = ft_substr(args[0], 0, eq_ptr - args[0]);
	if (!is_valid_identifier(var_name))
	{
		free(var_name);
		return (0);
	}
	remove_local_var(var_name, &context->local_vars);
	new_node = ft_lstnew(ft_strdup(args[0]));
	if (!new_node)
	{
		free(var_name);
		return (1);
	}
	ft_lstadd_back(&context->local_vars, new_node);
	free(var_name);
	return (0);
}

static void	redirect_list_last(t_list *redirect_list)
{
	t_list		*temp;

	while (redirect_list)
	{
		temp = redirect_list;
		redirect_list = redirect_list->next;
		free(temp);
	}
}

int	execute_simple_command(t_ast_node *node, t_shell_context *shell_context)
{
	t_ast_node	*cmd_node;
	t_list		*redirect_list;
	int			redirect_status;

	if (!node)
		return (0);
	redirect_list = NULL;
	cmd_node = collect_redirects(node, &redirect_list);
	redirect_status = apply_redirect_list(redirect_list);
	redirect_list_last(redirect_list);
	if (redirect_status != 0)
		return (1);
	if (!cmd_node)
		return (0);
	if (cmd_node->args && cmd_node->args[0] && ft_strchr(cmd_node->args[0], '=')
		&& !cmd_node->args[1])
		return (handle_variable_assignment(cmd_node->args, shell_context));
	if (!cmd_node->args || !cmd_node->args[0] || cmd_node->args[0][0] == '\0')
		return (0);
	if (is_builtin(cmd_node->args[0]))
		return (execute_builtin(cmd_node->args, shell_context));
	launch_command(cmd_node->args, shell_context->envp_cpy);
	return (127);
}
