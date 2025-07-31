#include "../../include/minishell.h"

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
	return (1);
}

int	execute_simple_command(t_ast_node *node, t_shell_context *shell_context)
{
	t_ast_node	*cmd_node;
	t_ast_node	*redirects[1024];
	int			i;
	int			j;

	i = 0;
	cmd_node = node;
	while (cmd_node && is_redirect_node(cmd_node->type))
	{
		redirects[i++] = cmd_node;
		cmd_node = cmd_node->left;
	}
	j = i - 1;
	while (j >= 0)
		if (apply_redirections(redirects[j--]) != 0)
			return (1);
	if (cmd_node && cmd_node->args && cmd_node->args[0]
		&& ft_strchr(cmd_node->args[0], '=') && !cmd_node->args[1])
		return (handle_variable_assignment(cmd_node->args, shell_context), 0);
	if (!cmd_node || !cmd_node->args || !cmd_node->args[0]
		|| cmd_node->args[0][0] == '\0')
		return (0);
	if (is_builtin(cmd_node->args[0]))
		return (execute_builtin(cmd_node->args, shell_context));
	return (launch_command(cmd_node->args, shell_context->envp_cpy), 127);
}
// t_ast_node *redirects[1024]; => Array para guardar punteros a nodos de redirect
// 1º while() => Recolectar todos los nodos de redirección y encontrar el
//		comando base.
// 2º while() => Aplicar las redirecciones en el orden en que se encontraron.
//		La última en la línea de comandos es la primera en el array, y será la
//		última en ser aplicada en este bucle, por lo que "gana".
// 2º if() después de los while( =ª) Ahora cmd_node apunta al comando real
//		y lo Ejecuta.
