#include "../../include/minishell.h"

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_HEREDOC);
}
/*
 * Propósito: Comprobar si un tipo de token corresponde a un operador de
 *            redirección (`<`, `>`, `<<`, `>>`).
 * Mecanismo: Compara el `type` con los enums de todos los tokens de
 *            redirección.
 * Llamado por:
 *   - `count_command_args`, `extract_args`: Para saber si deben
 * 		ignorar un token.
 *   - `parse_leading_redirects`, `apply_trailing_redirects`: Para saber si
 *     deben procesar un token como redirección.
*/

int	is_redirect_node(t_node_type type)
{
	return (type == NODE_REDIRECT_OUT
		|| type == NODE_REDIRECT_IN
		|| type == NODE_REDIRECT_APPEND
		|| type == NODE_HEREDOC);
}
/*
 * Propósito: Comprobar si un tipo de nodo del AST corresponde a un nodo de
 *            redirección.
 * Mecanismo: Compara el `type` con los enums de todos los nodos de
 *            redirección.
 * Llamado por: `parse_redirect_expression`, para saber hasta dónde recorrer
 *              la lista de nodos de redirección iniciales.
*/
