#include "../../include/minishell.h"

static t_ast_node	*parse_leading_redirects(t_parser *parser,
					t_ast_node **redirects_head)
{
	t_ast_node	*redirect_node;
	t_ast_node	*current_redirect;
	t_token		*redirect_token;
	t_token		*file_token;

	*redirects_head = NULL;
	current_redirect = NULL;
	while (parser->current && is_redirect_token(parser->current->type))
	{
		redirect_token = parser->current;
		parser->current = parser->current->next;
		file_token = consume_token(parser, TOKEN_WORD);
		if (!file_token)
			return (cleanup_ast(*redirects_head), NULL);
		redirect_node = create_redirect_node(redirect_token->type, NULL,
				file_token->value);
		if (!redirect_node)
			return (cleanup_ast(*redirects_head), NULL);
		if (!*redirects_head)
			*redirects_head = redirect_node;
		else
			current_redirect->left = redirect_node;
		current_redirect = redirect_node;
	}
	return (*redirects_head);
}
/*
 * Propósito: Parsear una secuencia de redirecciones que aparecen ANTES de un
 *            comando (ej: `< in.txt > out.txt cmd`).
 * Mecanismo:
 *   1. Itera mientras el token actual sea un operador de redirección.
 *   2. En cada iteración, consume el token de redirección y el siguiente
 *      token, que debe ser el nombre del archivo (`TOKEN_WORD`).
 *   3. Crea un nodo de redirección (`create_redirect_node`).
 *   4. Enlaza este nuevo nodo al final de la lista de redirecciones que se
 *      está construyendo. La lista se forma a través del puntero `left`.
 *   5. Devuelve la cabeza de la lista de nodos de redirección.
 * Llamado por: `parse_redirect_expression`.
 * Llama a:
 *   - `is_redirect_token`: Para controlar el bucle.
 *   - `consume_token`: Para obtener el nombre del archivo.
 *   - `create_redirect_node`: Para crear el nodo.
 *   - `cleanup_ast`: En caso de error de sintaxis.
*/

static t_ast_node	*apply_trailing_redirects(t_parser *parser,
											t_ast_node *base_node)
{
	t_ast_node	*new_node;
	t_token		*redirect_token;
	t_token		*file_token;

	while (parser->current && is_redirect_token(parser->current->type))
	{
		redirect_token = parser->current;
		parser->current = parser->current->next;
		file_token = consume_token(parser, TOKEN_WORD);
		if (!file_token)
			return (cleanup_ast(base_node), NULL);
		new_node = create_redirect_node(redirect_token->type, base_node,
				file_token->value);
		if (!new_node)
			return (cleanup_ast(base_node), NULL);
		base_node = new_node;
	}
	return (base_node);
}
/*
 * Propósito: Parsear y aplicar redirecciones que aparecen DESPUÉS de un
 *            comando (ej: `cmd arg1 > out.txt`).
 * Mecanismo:
 *   1. Itera mientras el token actual sea un operador de redirección.
 *   2. En cada iteración, consume el operador y el nombre del archivo.
 *   3. Crea un nuevo nodo de redirección (`create_redirect_node`), usando el
 *      `base_node` (que puede ser el comando u otra redirección) como su
 *      hijo izquierdo (`left`).
 *   4. Actualiza `base_node` para que apunte a este nuevo nodo. Esto crea una
 *      cadena donde cada redirección "envuelve" a la anterior.
 *   5. Devuelve el nodo más externo, que es la última redirección parseada.
 * Llamado por: `parse_redirect_expression`.
 * Llama a:
 *   - `is_redirect_token`: Para controlar el bucle.
 *   - `consume_token`: Para obtener el nombre del archivo.
 *   - `create_redirect_node`: Para crear y anidar el nodo.
 *   - `cleanup_ast`: En caso de error.
*/

t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*leading_redirects;
	t_ast_node	*command_node;
	t_ast_node	*final_node;
	t_ast_node	*current;

	parse_leading_redirects(parser, &leading_redirects); // 1. Parsear redirecciones que aparecen ANTES del comando
	command_node = parse_command(parser);				 // 2. Parsear el comando (si existe)
	if (!command_node && !leading_redirects)			 // 3. Si no hay comando ni redirecciones, es un error o línea vacía
		return (NULL);
	if (!command_node && leading_redirects) // 4. Si solo hay redirecciones (sin comando), es válido
		return (leading_redirects);
	final_node = apply_trailing_redirects(parser, command_node); // 5. Aplicar redirecciones que aparecen DESPUÉS del comando
	if (!final_node)
		return (cleanup_ast(leading_redirects), NULL);
	if (leading_redirects) // 6. Conectar las redirecciones iniciales con el resto
	{
		current = leading_redirects; // Encontrar el final de la cadena de redirecciones iniciales
		while (current->left && is_redirect_node(current->left->type))
			current = current->left;
		current->left = final_node; // Conectar el comando (con sus redirecciones posteriores) al final
		return (leading_redirects);
	}
	return (final_node);
}
/*
 * Propósito: Parsear una "expresión de redirección", que consiste en un
 *            comando simple opcionalmente rodeado por una o más redirecciones.
 *            Maneja casos como: `> out.txt cmd < in.txt arg1`.
 * Mecanismo:
 *   1. Llama a `parse_leading_redirects` para capturar todas las
 *      redirecciones que aparecen ANTES del comando.
 *   2. Llama a `parse_command` para obtener el nodo del comando y sus args.
 *   3. Valida los resultados: si no hay ni comando ni redirecciones, es una
 *      línea vacía o un error. Si solo hay redirecciones, es válido.
 *   4. Llama a `apply_trailing_redirects` para parsear las redirecciones que
 *      aparecen DESPUÉS del comando y las anida sobre el nodo de comando.
 *   5. Si había redirecciones iniciales, las conecta con el resto del árbol.
 *      Para ello, recorre la lista de `leading_redirects` hasta el final y
 *      enlaza el subárbol `final_node` a su puntero `left`.
 * Llamado por: `parse_pipe_expression`, ya que las redirecciones tienen mayor
 *              precedencia que los pipes.
 * Llama a:
 *   - `parse_leading_redirects`: Para las redirecciones iniciales.
 *   - `parse_command`: Para el comando central.
 *   - `apply_trailing_redirects`: Para las redirecciones finales.
 *   - `is_redirect_node`: Para recorrer la lista de redirecciones.
 *   - `cleanup_ast`: En caso de error de parseo.
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
		if (!new_node)
			return (cleanup_ast(left), cleanup_ast(right), NULL);
		left = new_node;
	}
	return (left);
}
/*
 * Propósito: Parsear expresiones de pipe ('|'), que tienen la menor
 *            precedencia de todos los operadores.
 * Mecanismo:
 *   1. Llama a `parse_redirect_expression` para obtener el subárbol del lado
 *      izquierdo (que es una unidad de mayor precedencia).
 *   2. Entra en un bucle que se ejecuta mientras el token actual sea `TOKEN_PIPE`.
 *   3. Dentro del bucle, consume el token '|'.
 *   4. Llama de nuevo a `parse_redirect_expression` para obtener el subárbol
 *      del lado derecho.
 *   5. Crea un `NODE_PIPE` que une los subárboles `left` y `right`.
 *   6. Actualiza `left` para que apunte a este nuevo nodo de pipe, manejando
 *      así la asociatividad a la izquierda (ej: `a | b | c` se agrupa como
 *      `(a | b) | c`).
 * Llamado por: `parse`, ya que es el punto de partida del análisis de expresiones.
 * Llama a:
 *   - `parse_redirect_expression`: Para parsear los operandos.
 *   - `consume_token_type`: Para consumir el token `|`.
 *   - `create_binary_node`: Para construir el nodo de pipe.
 *   - `cleanup_ast`: Para liberar memoria en caso de error.
*/
