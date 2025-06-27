#include "../../include/minishell.h"

static int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}
/*
 * Propósito: Comprobar si un tipo de token corresponde a un operador de
 *            redirección.
 * Mecanismo: Compara el `type` con los enums de todos los tokens de
 *            redirección.
 * Llamado por: `parse_redirect_expression`, para decidir si debe continuar
 *              en el bucle de parseo de redirecciones.
*/

t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*new_node;
	t_token		*redirect_token;
	t_token		*file_token;

	left = parse_command(parser);
	if (!left)
		return (NULL);
	while (parser->current && is_redirect_token(parser->current->type))
	{
		redirect_token = parser->current;
		parser->current = parser->current->next;
		file_token = consume_token(parser, TOKEN_WORD);
		if (!file_token)
		{
			cleanup_ast(left);
			return (NULL);
		}
		new_node = create_redirect_node(redirect_token->type, left, file_token->value);
		if (!new_node)
			return (cleanup_ast(left), NULL);
		left = new_node;
	}
	return (left);
}
/*
 * Propósito: Parsear expresiones de redirección ('>', '<', '>>', '<<'), que
 *            tienen mayor precedencia que los pipes.
 * Mecanismo:
 *   1. Primero, llama a `parse_command` para obtener el nodo de comando base.
 *   2. Entra en un bucle que se ejecuta mientras el token actual sea de
 *      redirección. Esto permite manejar múltiples redirecciones (ej: cmd > a < b).
 *   3. Dentro del bucle, consume el token de redirección y el token de palabra
 *      (el nombre del archivo).
 *   4. Crea un nuevo nodo de redirección que "envuelve" al nodo `left` actual.
 *   5. Actualiza `left` para que apunte a este nuevo nodo. De esta forma, los
 *      redirects se anidan correctamente.
 * Llamado por: `parse_pipe_expression`, ya que los redirects se evalúan antes
 *              que los pipes.
 * Llama a:
 *   - `parse_command`: Para obtener el comando a redirigir.
 *   - `is_redirect_token`: Para la condición del bucle.
 *   - `consume_token`: Para obtener el nombre del archivo.
 *   - `create_redirect_node`: Para construir el nodo en el AST.
 *   - `cleanup_ast`: Para liberar memoria en caso de error.
 * La variable temporal "new_node" se crea para que en el caso de fallar "create_direct_node" podamos liberar "left"
*/

static t_token	*consume_token_type(t_parser *parser, t_token_type type)
{
	t_token	*token;

	if (!parser->current || parser->current->type != type)
	{
		parser->error = 1;
		return (NULL);
	}
	token = parser->current;
	parser->current = parser->current->next;
	return (token);
}
/*
 * Propósito: Consumir el token actual si y solo si coincide con un tipo
 *            esperado.
 * Mecanismo:
 *   1. Comprueba si el token actual existe y si su tipo es el esperado.
 *   2. Si no coincide, activa el flag de error del parser y retorna NULL.
 *   3. Si coincide, guarda el token, avanza el parser y lo retorna.
 * Llamado por: `parse_pipe_expression`, para consumir específicamente el
 *              token de `|`.
*/

t_ast_node	*parse_pipe_expression(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*new_node;
	t_token		*pipe_token;

	left = parse_redirect_expression(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == TOKEN_PIPE)
	{
		pipe_token = consume_token_type(parser, TOKEN_PIPE);
		if (!pipe_token)
			return (cleanup_ast(left), NULL);
		right = parse_redirect_expression(parser);
		if (!right)
			return (cleanup_ast(left), NULL);
		new_node = create_binary_node(TOKEN_PIPE, left, right);
		if (!left)
			return (cleanup_ast(left), cleanup_ast(right), NULL);
		left = new_node;
	}
	return (left);
}
/*
 * Propósito: Parsear expresiones de pipe ('|'), que tienen la menor
 *            precedencia de operadores.
 * Mecanismo:
 *   1. Llama a `parse_redirect_expression` para obtener el subárbol del lado
 *      izquierdo (que puede ser un comando o un comando con redirects).
 *   2. Entra en un bucle que se ejecuta mientras el token actual sea `TOKEN_PIPE`.
 *   3. Dentro del bucle, consume el token de pipe.
 *   4. Llama de nuevo a `parse_redirect_expression` para obtener el subárbol
 *      del lado derecho.
 *   5. Crea un `NODE_PIPE` que une los subárboles `left` y `right`.
 *   6. Actualiza `left` para que apunte a este nuevo nodo de pipe, manejando
 *      correctamente la asociatividad a la izquierda (ej: a | b | c).
 * Llamado por: `parse`, ya que es el punto de partida del análisis de
 *              expresiones.
 * Llama a:
 *   - `parse_redirect_expression`: Para parsear los operandos de mayor
 *     precedencia a cada lado del pipe.
 *   - `consume_token_type`: Para consumir el token `|`.
 *   - `create_binary_node`: Para construir el nodo de pipe.
 *   - `cleanup_ast`: Para liberar memoria en caso de error.
 * La variable temporal "new_node" se crea para que en el caso de fallar "create_binary_node" podamos liberar "left"
*/
