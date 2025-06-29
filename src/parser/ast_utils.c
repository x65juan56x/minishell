#include "../../include/minishell.h"

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = NULL;
	node->file = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}
/*
 * Propósito: Función de fábrica genérica para crear un nodo de AST,
 *            inicializando todos sus miembros de forma segura.
 * Mecanismo:
 *   1. Aloca memoria para la estructura `t_ast_node`.
 *   2. Asigna el `type` del nodo.
 *   3. Inicializa todos los punteros (`args`, `file`, `left`, `right`) a NULL
 *      para prevenir accesos a memoria inválida y facilitar la limpieza.
 * Llamado por: `create_binary_node`, `create_redirect_node`, `parse_command`.
 *              Es la base para la creación de cualquier nodo del árbol.
 * Llama a: `malloc`.
*/

static t_node_type	token_to_node_type(t_token_type token_type)
{
	if (token_type == TOKEN_PIPE)
		return (NODE_PIPE);
	if (token_type == TOKEN_REDIRECT_IN)
		return (NODE_REDIRECT_IN);
	if (token_type == TOKEN_REDIRECT_OUT)
		return (NODE_REDIRECT_OUT);
	if (token_type == TOKEN_REDIRECT_APPEND)
		return (NODE_REDIRECT_APPEND);
	if (token_type == TOKEN_HEREDOC)
		return (NODE_HEREDOC);
	return (NODE_COMMAND);
}
/*
 * Propósito: Mapear un tipo de token de operador a su tipo de nodo de AST
 *            correspondiente.
 * Mecanismo: Una serie de comparaciones `if` que devuelven el `e_node_type`
 *            adecuado para un `e_token_type` de operador dado.
 * Llamado por: `create_binary_node` y `create_redirect_node`, para
 *              determinar qué tipo de nodo crear.
*/

t_ast_node	*create_binary_node(t_token_type op_type, t_ast_node *left,
			t_ast_node *right)
{
	t_ast_node	*node;

	node = create_ast_node(token_to_node_type(op_type));
	if (!node)
		return (NULL);
	node->left = left;
	node->right = right;
	return (node);
}
/*
 * Propósito: Función de fábrica especializada para crear nodos de operadores
 *            binarios (como `|`).
 * Mecanismo:
 *   1. Llama a `token_to_node_type` para convertir el tipo de token a tipo de nodo.
 *   2. Crea un nodo base con `create_ast_node`.
 *   3. Asigna los subárboles `left` y `right` a los hijos del nuevo nodo.
 * Llamado por: `parse_pipe_expression`, para construir los nodos de pipe.
 * Llama a: `create_ast_node`, `token_to_node_type`.
*/

t_ast_node	*create_redirect_node(t_token_type redirect_type, t_ast_node *cmd,
			char *file)
{
	t_ast_node	*node;

	node = create_ast_node(token_to_node_type(redirect_type));
	if (!node)
		return (NULL);
	node->left = cmd;
	node->file = ft_strdup(file);
	if (!node->file)
	{
		free(node);
		return (NULL);
	}
	return (node);
}
/*
 * Propósito: Función de fábrica especializada para crear nodos de redirección.
 * Mecanismo:
 *   1. Llama a `token_to_node_type` para convertir el tipo de token a tipo de nodo.
 *   2. Crea un nodo base.
 *   3. Asigna el subárbol del comando al hijo `left`. El hijo `right` no se usa.
 *   4. Duplica el string del nombre de archivo y lo asigna al campo `file`.
 *      La duplicación es clave para que el AST tenga su propia copia.
 * Llamado por: `parse_leading_redirects` y `apply_trailing_redirects`.
 * Llama a:
 *   - `create_ast_node`: Para el nodo base.
 *   - `token_to_node_type`: Para obtener el tipo de nodo.
 *   - `ft_strdup`: Para copiar el nombre del archivo.
 *   - `free`: En caso de que `ft_strdup` falle.
*/
