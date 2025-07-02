#include "../../include/minishell.h"

static int	find_heredoc_in_node(t_ast_node *node)
{
	if (!node)
		return (-1);
	if (node->type == NODE_HEREDOC)
	{
		printf("[heredoc_preprocessor] Encontrado NODE_HEREDOC, ejecutando heredoc para: %s\n", node->file);
		return (execute_heredoc(node->file));
	}
	return (-1);
}
/*
 * Propósito: Detectar y ejecutar un here-doc en un nodo dado.
 * Mecanismo:
 *   1. Si el nodo es NULL, devuelve -1.
 *   2. Si `node->type == NODE_HEREDOC`, llama a `execute_heredoc`
 *      con el delimitador (`node->file`) y retorna el fd de lectura.
 *   3. Si no es un here-doc, devuelve -1.
 * Llamado por: `search_heredoc_recursive`.
 * Llama a:
 *   - `execute_heredoc`
 */

static int	search_heredoc_recursive(t_ast_node *node)
{
	int	heredoc_fd;

	if (!node)
		return (-1);
	heredoc_fd = find_heredoc_in_node(node);
	if (heredoc_fd != -1)
		return (heredoc_fd);
	if (node->left)
	{
		heredoc_fd = search_heredoc_recursive(node->left);
		if (heredoc_fd != -1)
			return (heredoc_fd);
	}
	return (-1);
}
/*
 * Propósito: Recorrer el subárbol para hallar el primer here-doc.
 * Mecanismo:
 *   1. Llama a `find_heredoc_in_node` en el nodo actual.
 *   2. Si retorna fd ≥ 0, lo propaga hacia arriba.
 *   3. Si no, y existe `node->left`, recorre recursivamente a la izquierda.
 *   4. Devuelve -1 si no encuentra ningún here-doc.
 * Llamado por: `preprocess_heredocs`.
 * Llama a:
 *   - `find_heredoc_in_node`
 */

int	preprocess_heredocs(t_ast_node *node)
{
	printf("[heredoc_preprocessor] Entrando en preprocess_heredocs\n");
	int fd = search_heredoc_recursive(node);
	printf("[heredoc_preprocessor] fd devuelto: %d\n", fd);
	return fd;
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
