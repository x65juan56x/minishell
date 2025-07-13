#include "../../include/minishell.h"

int execute_simple_command(t_ast_node *node, char ***envp_ptr)
{
	t_ast_node *cmd_node = node;
	t_ast_node *redirects[1024]; // Array para guardar punteros a nodos de redirect
	int i;
	int j;

	i = 0;
	while (cmd_node && is_redirect_node(cmd_node->type))  // 1. Recolectar todos los nodos de redirección y encontrar el comando base.
	{
		redirects[i++] = cmd_node;
		cmd_node = cmd_node->left;
	}
	j = i - 1; // 2. Aplicar las redirecciones en el orden en que se encontraron.
	while (j >= 0) // La última en la línea de comandos es la primera en el array, y será la última en ser aplicada en este bucle, por lo que "gana".
	{
		if (apply_redirections(redirects[j]) != 0)
			return (1);
		j--;
	}
	if (!cmd_node || !cmd_node->args || !cmd_node->args[0] // 3. Ahora cmd_node apunta al comando real. Ejecutarlo.
		|| cmd_node->args[0][0] == '\0')
		return (0);
	if (is_builtin(cmd_node->args[0]))
		return (execute_builtin(cmd_node->args, envp_ptr));
	launch_command(cmd_node->args, *envp_ptr);
	return (127);
}
