#include "../../include/minishell.h"

static int	count_command_args(t_parser *parser)
{
	t_token	*temp;
	int		count;

	count = 0;
	temp = parser->current;
	while (temp && temp->type != TOKEN_PIPE && temp->type != TOKEN_EOF)
	{
		if (temp->type == TOKEN_WORD)
			count++;
		else if (is_redirect_token(temp->type))
		{
			temp = temp->next;	// Saltar el operador de redirección
			if (temp && temp->type == TOKEN_WORD)	// Saltar el archivo de la redirección (debe ser TOKEN_WORD)
				temp = temp->next;
			continue ;
		}
		else
			break ;	// Otro tipo de token que no sabemos manejar
		temp = temp->next;
	}
	return (count);
}
/*
 * Propósito: Contar el número de argumentos (tokens de tipo TOKEN_WORD) que
 *            pertenecen a un comando simple, ignorando los operadores de
 *            redirección y sus archivos asociados.
 * Mecanismo:
 *   1. Itera sobre los tokens desde la posición actual del parser.
 *   2. El bucle se detiene si encuentra un pipe, el final de los tokens, o
 *      cualquier otro token que no sea una palabra o una redirección.
 *   3. Si el token es de tipo `TOKEN_WORD`, incrementa el contador.
 *   4. Si es un token de redirección, salta tanto el token del operador como
 *      el siguiente token (que se asume es el nombre del archivo).
 *   5. Devuelve el número total de palabras contadas.
 * Llamado por: `collect_command_args`, para saber cuánta memoria alocar.
 * Llama a: `is_redirect_token`.
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

static char	**collect_command_args(t_parser *parser)
{
	char	**args;
	int		count;
	int		got;
	t_token	*temp;

	count = count_command_args(parser);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(*args) * (count + 1));
	if (!args)
		return (NULL);
	temp = parser->current;
	got = extract_args(args, count, &temp);
	if (got < 0)
		return (ft_freearr(args), NULL);
	args[got] = NULL;
	while (parser->current && parser->current->type == TOKEN_WORD)
		parser->current = parser->current->next;
	return (args);
}
/*
 * Propósito: Recolectar todos los argumentos de un comando en un array de
 *            strings (char **), que es el formato que espera `execve`.
 * Mecanismo:
 *   1. Llama a `count_command_args` para determinar el tamaño del array.
 *   2. Aloca memoria para el array de punteros más un terminador NULL.
 *   3. Llama a `extract_args` (de `parser_utils.c`) para que llene el array
 *      con los valores de los tokens, saltando las redirecciones.
 *   4. `extract_args` también actualiza un puntero temporal `temp` a la
 *      posición del primer token no consumido.
 *   5. Termina el array con un puntero NULL.
 *   6. Avanza el puntero `parser->current` para que apunte al primer token
 *      que no forma parte de los argumentos iniciales del comando.
 * Llamado por: `parse_command`.
 * Llama a:
 *   - `count_command_args`: Para pre-calcular el tamaño.
 *   - `malloc`: Para alocar el array.
 *   - `extract_args`: Para llenar el array con los argumentos.
 *   - `ft_freearr`: Para limpiar en caso de error.
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
		return (free(node), NULL);
	return (node);
}
/*
 * Propósito: Parsear la unidad más fundamental de la gramática: un comando
 *            simple y sus argumentos. Es el nivel de mayor precedencia.
 * Mecanismo:
 *   1. Comprueba si el token actual es una palabra (`TOKEN_WORD`). Si no, no
 *      es un comando simple y retorna NULL.
 *   2. Crea un nodo base de tipo `NODE_COMMAND`.
 *   3. Llama a `collect_command_args` para que recolecte todos los argumentos
 *      y los asigne a `node->args`.
 *   4. Si la recolección falla, limpia el nodo creado y retorna NULL.
 * Llamado por: `parse_redirect_expression`, ya que un comando es la entidad
 *              que puede ser redirigida.
 * Llama a:
 *   - `create_ast_node`: Para crear el nodo del comando.
 *   - `collect_command_args`: Para obtener su array de argumentos.
 *   - `free`: Para limpiar en caso de error.
*/
