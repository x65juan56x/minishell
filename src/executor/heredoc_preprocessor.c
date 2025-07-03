#include "../../include/minishell.h"

static int	process_heredoc_node(t_ast_node **node_ptr)
{
	t_ast_node	*heredoc_node;
	t_ast_node	*command_node;
	int			heredoc_fd;

	heredoc_node = *node_ptr;
	if (heredoc_node->type != NODE_HEREDOC)
		return (-1);
	heredoc_fd = execute_heredoc(heredoc_node->file);
	if (heredoc_fd < 0)
		return (-1);
	// Extraer el comando subyacente
	command_node = heredoc_node->left;
	heredoc_node->left = NULL; // Desconectar para evitar double-free
	// Reemplazar el nodo heredoc por el comando
	*node_ptr = command_node;
	// Limpiar el nodo heredoc (sin sus hijos)
	if (heredoc_node->file)
		free(heredoc_node->file);
	free(heredoc_node);
	
	return (heredoc_fd);
}

static int	search_and_process_heredoc(t_ast_node **node_ptr)
{
	t_ast_node	*node;
	int			heredoc_fd;

	if (!node_ptr || !*node_ptr)
		return (-1);
	node = *node_ptr;
	// Si este nodo es un heredoc, procesarlo
	if (node->type == NODE_HEREDOC)
		return (process_heredoc_node(node_ptr));
	// Buscar recursivamente en el hijo izquierdo
	if (node->left)
	{
		heredoc_fd = search_and_process_heredoc(&(node->left));
		if (heredoc_fd != -1)
			return (heredoc_fd);
	}
	return (-1);
}

int	preprocess_heredocs(t_ast_node **node_ptr)
{
	return (search_and_process_heredoc(node_ptr));
}
/*
 * Propósito: Iniciar el preprocesamiento de here-docs en el AST.
 * Mecanismo:
 *   1. Llama a `search_heredoc_recursive` con la raíz del AST.
 *   2. Devuelve el fd de lectura del primer here-doc o -1 si no hay ninguno.
 * Llamado por: `execute_pipe_node` antes de crear la tubería.
 * Llama a:
 *   - `search_heredoc_recursive`
 */
