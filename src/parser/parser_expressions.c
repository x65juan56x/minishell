#include "../../include/minishell.h"

// int	is_redirect_token(t_token_type type)
// {
// 	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
// 		|| type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
// }


static t_ast_node	*parse_leading_redirects(t_parser *parser, t_ast_node **redirects_head)
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
		redirect_node = create_redirect_node(redirect_token->type, NULL, file_token->value);
		if (!redirect_node)
			return (cleanup_ast(*redirects_head), NULL);
		if (!*redirects_head)
		{
			*redirects_head = redirect_node;
			current_redirect = redirect_node;
		}
		else
		{
			current_redirect->left = redirect_node;
			current_redirect = redirect_node;
		}
	}
	return (*redirects_head);
}

// NUEVA FUNCIÓN: Aplica redirecciones que aparecen DESPUÉS del comando
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

// FUNCIÓN PRINCIPAL COMPLETAMENTE REDISEÑADA
t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*leading_redirects;
	t_ast_node	*command_node;
	t_ast_node	*final_node;
	t_ast_node	*current;

	parse_leading_redirects(parser, &leading_redirects);	// 1. Parsear redirecciones que aparecen ANTES del comando
	command_node = parse_command(parser);	// 2. Parsear el comando (si existe)
	if (!command_node && !leading_redirects)	// 3. Si no hay comando ni redirecciones, es un error o línea vacía
		return (NULL);
	if (!command_node && leading_redirects)	// 4. Si solo hay redirecciones (sin comando), es válido
		return (leading_redirects);
	final_node = apply_trailing_redirects(parser, command_node);	// 5. Aplicar redirecciones que aparecen DESPUÉS del comando
	if (!final_node)
		return (cleanup_ast(leading_redirects), NULL);
	if (leading_redirects)	// 6. Conectar las redirecciones iniciales con el resto
	{
		current = leading_redirects;	// Encontrar el final de la cadena de redirecciones iniciales
		while (current->left && (current->left->type == NODE_REDIRECT_OUT || 
			current->left->type == NODE_REDIRECT_IN || 
			current->left->type == NODE_REDIRECT_APPEND || 
			current->left->type == NODE_HEREDOC))
			current = current->left;
		current->left = final_node;	// Conectar el comando (con sus redirecciones posteriores) al final
		return (leading_redirects);
	}
	return (final_node);
}

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
