#include "../../include/minishell.h"

/*
 * Comprueba si un token es operador de redirección:
 *   <, >, >> o <<.
 */
int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_HEREDOC);
}
/*
 * Propósito: Comprobar si un tipo de token corresponde a un operador de
 *            redirección.
 * Mecanismo: Compara el `type` con los enums de todos los tokens de
 *            redirección.
 * Llamado por: `parse_redirect_expression`, para decidir si debe continuar
 *              en el bucle de parseo de redirecciones.
*/
