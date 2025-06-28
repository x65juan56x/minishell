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
		else if (temp->type == TOKEN_REDIRECT_IN || temp->type == TOKEN_REDIRECT_OUT || 
				temp->type == TOKEN_REDIRECT_APPEND || temp->type == TOKEN_HEREDOC)
		{
			// Saltar el operador de redirección
			temp = temp->next;
			// Saltar el archivo de la redirección (debe ser TOKEN_WORD)
			if (temp && temp->type == TOKEN_WORD)
				temp = temp->next;
			continue;
		}
		else
			break; // Otro tipo de token que no sabemos manejar
		temp = temp->next;
	}
	return (count);
}

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
	t_token	*temp;

	count = count_command_args(parser);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	temp = parser->current;
	i = 0;
	while (i < count && temp && temp->type != TOKEN_PIPE && temp->type != TOKEN_EOF)
	{
		if (temp->type == TOKEN_WORD)
		{
			args[i] = ft_strdup(temp->value);
			if (!args[i])
				return (ft_freearr(args), NULL);
			i++;
		}
		else if (is_redirect_token(temp->type))
		{
			temp = temp->next;
			if (temp && temp->type == TOKEN_WORD)
				temp = temp->next;
			continue;
		}
		else
			break;
		temp = temp->next;
	}
	while (parser->current && parser->current->type == TOKEN_WORD)
		parser->current = parser->current->next;
	args[i] = NULL;
	return (args);
}

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
