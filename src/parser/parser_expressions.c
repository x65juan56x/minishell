#include "../../include/minishell.h"

static int	parse_args_and_redirect(t_ast_node **node, char **args,
		int *arg_count, t_parser *parser)
{
	t_token	*op;
	t_token	*file;

	if (parser->current->type == TOKEN_WORD)
		args[(*arg_count)++] = ft_strdup(consume_token
				(parser, TOKEN_WORD)->value);
	else if (is_redirect_token(parser->current->type))
	{
		op = consume_token(parser, parser->current->type);
		file = consume_token(parser, TOKEN_WORD);
		if (!file)
		{
			parser->error = 1;
			ft_freearr(args);
			return (cleanup_ast(*node), -1);
		}
		*node = create_redirect_node(op->type, *node, file->value);
		if (!*node)
		{
			parser->error = 1;
			ft_freearr(args);
			return (-1);
		}
	}
	return (0);
}

static char	**create_compact_args(char **args, int *arg_count)
{
	char	**compact_args;
	int		i;

	i = 0;
	compact_args = malloc(sizeof(char *) * (*arg_count + 2));
	if (!compact_args)
		return (NULL);
	if (arg_count == 0)
	{
		compact_args[0] = ft_strdup("");
		compact_args[1] = NULL;
		return (compact_args);
	}
	i = 0;
	while (i < *arg_count)
	{
		compact_args[i] = ft_strdup(args[i]);
		if (!compact_args[i])
			return (ft_freearr(compact_args), NULL);
		i++;
	}
	compact_args[*arg_count] = NULL;
	return (compact_args);
}

t_ast_node	*parse_redirect_expression(t_parser *parser)
{
	t_ast_node	*node;
	t_ast_node	*cmd_node;
	char		**args;
	int			arg_count;

	if (parser->current && parser->current->type != TOKEN_WORD
		&& !is_redirect_token(parser->current->type))
		return (consume_token_type(parser, TOKEN_WORD), NULL);
	cmd_node = create_ast_node(NODE_COMMAND);
	if (!cmd_node)
		return (NULL);
	args = ft_calloc(1024, sizeof(char *));
	if (!args)
		return (cleanup_ast(cmd_node), NULL);
	arg_count = 0;
	node = cmd_node;
	while (parser->current && (parser->current->type == TOKEN_WORD
			|| is_redirect_token(parser->current->type)))
		if (parse_args_and_redirect(&node, args, &arg_count, parser) != 0)
			return (NULL);
	cmd_node->args = create_compact_args(args, &arg_count);
	if (!cmd_node->args)
		return (ft_freearr(args), cleanup_ast(node), NULL);
	return (ft_freearr(args), node);
}
// Si la expresión no empieza con una palabra o una redirección, es un error.
//		Llamamos a consume_token_type para que genere el mensaje de error estándar.
//		consume_token_type fallará y establecerá el error.
// 1. Primero, creamos un nodo de comando vacío.
//		Usamos un array temporal grande para los argumentos.
// 2. Bucle principal: consume palabras y redirecciones.
// if (arg_count == 0) => Si no hubo palabras, el comando es inválido.
//		Pero si hubo redirecciones, el nodo principal no es el de comando.
//		Le damos un argumento vacío para que el ejecutor no lo ignore.

t_ast_node	*parse_parenthesis_expression(t_parser *parser)
{
	t_ast_node	*expr;

	if (!consume_token_type(parser, TOKEN_LPAREN))
		return (NULL);
	expr = parse_logical_expression(parser);
	if (!expr)
		return (NULL);
	if (!consume_token_type(parser, TOKEN_RPAREN))
		return (cleanup_ast(expr), NULL);
	return (expr);
}

t_ast_node	*parse_primary_expression(t_parser *parser)
{
	if (parser->current && parser->current->type == TOKEN_LPAREN)
		return (parse_parenthesis_expression(parser));
	return (parse_redirect_expression(parser));
}

t_ast_node	*parse_pipe_expression(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*new_node;
	t_token		*pipe_token;

	left = parse_primary_expression(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == TOKEN_PIPE)
	{
		pipe_token = consume_token_type(parser, TOKEN_PIPE);
		if (!pipe_token)
			return (cleanup_ast(left), NULL);
		right = parse_primary_expression(parser);
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

t_ast_node	*parse_logical_expression(t_parser *parser)
{
	t_ast_node		*left;
	t_ast_node		*right;
	t_ast_node		*new_node;
	t_token_type	op_type;

	left = parse_pipe_expression(parser);
	if (!left)
		return (NULL);
	while (parser->current && (parser->current->type == TOKEN_AND
			|| parser->current->type == TOKEN_OR))
	{
		op_type = parser->current->type;
		consume_token_type(parser, op_type);
		if (parser->error)
			return (cleanup_ast(left), NULL);
		right = parse_pipe_expression(parser);
		if (!right)
			return (cleanup_ast(left), NULL);
		new_node = create_binary_node(op_type, left, right);
		if (!new_node)
			return (cleanup_ast(left), cleanup_ast(right), NULL);
		left = new_node;
	}
	return (left);
}
