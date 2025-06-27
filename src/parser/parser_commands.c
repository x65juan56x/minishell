#include "../../include/minishell.h"

static int	count_command_args(t_parser *parser)
{
	t_token	*temp;
	int		count;

	count = 0;
	temp = parser->current;
	while (temp && temp->type == TOKEN_WORD)
	{
		count++;
		temp = temp->next;
	}
	return (count);
}
/*
 * Propósito: Contar cuántos tokens de tipo `TOKEN_WORD` consecutivos hay a
 *            partir de la posición actual del parser.
 * Mecanismo: Utiliza un puntero temporal para recorrer la lista de tokens sin
 *            modificar el estado del parser. Incrementa un contador por cada
 *            `TOKEN_WORD` hasta que encuentra un token de otro tipo o el final.
 * Llamado por: `collect_command_args`, para saber cuánta memoria necesita
 *              alocar para el array de argumentos, evitando reallocs.
*/

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
 * Propósito: Consumir el token actual si coincide con el tipo esperado y
 *            avanzar el puntero del parser.
 * Mecanismo:
 *   1. Valida que el token actual exista y sea del tipo `expected`.
 *   2. Si no lo es, marca un error de sintaxis en el parser y retorna NULL.
 *   3. Si coincide, guarda el token, avanza el parser al siguiente y retorna
 *      el token guardado.
 * Llamado por:
 *   - `collect_command_args`: Para consumir cada `TOKEN_WORD` del comando.
 *   - `parse_redirect_expression`: Para consumir el `TOKEN_WORD` del archivo.
*/

static char	**collect_command_args(t_parser *parser)
{
	char	**args;
	int		count;
	int		i;
	t_token	*token;

	count = count_command_args(parser);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		token = consume_token(parser, TOKEN_WORD);
		if (!token)
			return (ft_freearr(args), NULL);
		args[i] = ft_strdup(token->value);
		if (!args[i])
			return (ft_freearr(args), NULL);
		i++;
	}
	args[i] = NULL;
	return (args);
}
/*
 * Propósito: Recolectar todos los tokens de palabra consecutivos y
 *            construir un array de strings (char **) terminado en NULL,
 *            apto para `execve`.
 * Mecanismo:
 *   1. Llama a `count_command_args` para pre-calcular el tamaño necesario.
 *   2. Aloca la memoria para el array de punteros.
 *   3. Itera `count` veces, consumiendo cada `TOKEN_WORD` y duplicando su
 *      valor (`token->value`) en el array `args`.
 *   4. Añade el `NULL` terminador al final del array.
 * Llamado por: `parse_command`, para obtener la lista de argumentos del
 *              comando.
 * Llama a: `count_command_args`, `malloc`, `consume_token`, `ft_strdup`,
 *          `ft_freearr` (para limpieza en caso de error).
*/

t_ast_node	*parse_command(t_parser *parser)
{
	t_ast_node	*node;

	if (!parser->current || parser->current->type != TOKEN_WORD)
		return (NULL);
	node = create_ast_node(NODE_COMMAND);
	if (!node)
		return (NULL);
	node->args = collect_command_args(parser);
	if (!node->args)
	{
		free(node);
		parser->error = 1;
		return (NULL);
	}
	return (node);
}
/*
 * Propósito: Parsear la unidad más fundamental de la gramática: un comando
 *            simple y sus argumentos. Es el nivel de mayor precedencia.
 * Mecanismo:
 *   1. Comprueba si el token actual es una palabra. Si no, no es un comando
 *      simple y retorna NULL.
 *   2. Crea un nodo base de tipo `NODE_COMMAND`.
 *   3. Llama a `collect_command_args` para que recolecte todos los argumentos
 *      y los asigne a `node->args`.
 *   4. Si la recolección falla, limpia el nodo creado y marca un error.
 * Llamado por: `parse_redirect_expression`, ya que un comando es la entidad
 *              que puede ser redirigida.
 * Llama a: `create_ast_node`, `collect_command_args`, `free`.
*/
