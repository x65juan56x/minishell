#include "../../include/minishell.h"

static char	*build_heredoc_filename(int heredoc_id)
{
	char	*filename;
	char	*pid_str;
	char	*id_str;
	char	*temp;

	pid_str = ft_itoa(getpid());
	id_str = ft_itoa(heredoc_id);
	temp = ft_strjoin("/tmp/minishell-", pid_str);
	free(pid_str);
	if (!temp)
		return (free(id_str), NULL);
	filename = ft_strjoin(temp, id_str);
	free(temp);
	free(id_str);
	return (filename);
}

int	preprocess_heredocs(t_ast_node **node_ptr, int *heredoc_id_ptr)
{
	t_ast_node	*node;
	char		*temp_filename;

	if (!node_ptr || !*node_ptr)
		return (0);
	node = *node_ptr;
	if (node->left) // Búsqueda recursiva en post-orden (hijos primero)
		if (preprocess_heredocs(&(node->left), heredoc_id_ptr) != 0)
			return (-1);
	if (node->right)
		if (preprocess_heredocs(&(node->right), heredoc_id_ptr) != 0)
			return (-1);
	if (node->type == NODE_HEREDOC) // Si el nodo actual es un heredoc, lo procesamos.
	{
		if (execute_heredoc(node->file, heredoc_id_ptr) != 0)
			return (-1); // Error o cancelación
		temp_filename = build_heredoc_filename(*heredoc_id_ptr); // Creamos el nombre del fichero temporal.
		if (!temp_filename)
			return (-1);
		(*heredoc_id_ptr)++;
		free(node->file); // Transformamos el nodo
		node->file = temp_filename;
		node->type = NODE_REDIRECT_IN; // Ahora es una redirección de entrada normal.
	}
	return (0);
}
