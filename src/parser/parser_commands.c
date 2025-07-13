#include "../../include/minishell.h"

t_token	*consume_token(t_parser *parser, t_token_type expected)
{
	t_token	*token;

	if (!parser->current || parser->current->type != expected)
	{
		parser->error = 1;
		return (NULL);
	}
	token = parser->current;
	parser->current = parser->current->next;
	return (token);
}
/*
 * Propósito: Consumir el token actual si coincide con un tipo esperado y
 *            avanzar el puntero del parser. Es un validador y consumidor.
 * Mecanismo:
 *   1. Valida que el token actual exista y sea del tipo `expected`.
 *   2. Si no lo es, marca un error de sintaxis en el parser y retorna NULL.
 *   3. Si coincide, guarda el token actual, avanza el parser al siguiente y
 *      retorna el token guardado para su uso.
 * Llamado por: `parse_leading_redirects` y `apply_trailing_redirects` para
 *              consumir el `TOKEN_WORD` del nombre de archivo.
*/
