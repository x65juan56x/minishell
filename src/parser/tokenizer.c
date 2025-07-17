#include "../../include/minishell.h"

static t_token	*process_operator(const char *input, int *i)
{
	int				advance;
	t_token_type	type;
	char			*value;

	advance = 0;
	type = get_operator_type(&input[*i], &advance);
	value = ft_substr(input, *i, advance);
	if (!value)
		return (NULL);
	*i += advance;
	return (create_token(type, value));
}
/*
 * Propósito: Procesar un token de operador (como |, >, >>, etc.).
 * Mecanismo:
 *   1. Llama a `get_operator_type` para determinar el tipo exacto de operador
 *      y cuántos caracteres ocupa (1 o 2).
 *   2. Extrae el valor del operador (ej: ">>") usando `ft_substr`.
 *   3. Avanza el índice principal `i` la cantidad de caracteres consumida.
 *   4. Llama a `create_token` para crear el nodo del token con el tipo y valor.
 * Llamado por: `generate_token_list`, cuando encuentra un carácter que podría
 *              ser un operador.
 * Llama a:
 *   - `get_operator_type`: Para identificar el operador.
 *   - `ft_substr`: Para crear una copia del string del operador.
 *   - `create_token`: Para ensamblar el token final.
*/

static t_token	*process_word(const char *input, int *i)
{
	char	*value;
	t_token	*token;
	int		quoted;

	quoted = 0;
	value = extract_word_token(input, i, &quoted);
	if (!value)
		return (NULL);
	token = create_token(TOKEN_WORD, value);
	if (quoted > 0)
		token->in_quotes = 1;
	else
		is_expand_needed (value, quoted, token);
	return (token);
}
/*
 * Propósito: Procesar un token de palabra (un comando, un argumento, etc.).
 * Mecanismo:
 *   1. Delega la compleja tarea de extraer la palabra (manejando comillas)
 *      a la función `extract_word_token`.
 *   2. `extract_word_token` devuelve el valor procesado (sin comillas externas)
 *      y actualiza el índice `i`.
 *   3. Llama a `create_token` para crear un token de tipo `TOKEN_WORD`.
 * Llamado por: `generate_token_list`, cuando el carácter actual no es un
 *              operador.
 * Llama a:
 *   - `extract_word_token`: Para manejar la lógica de extracción de palabras
 *     y comillas.
 *   - `create_token`: Para ensamblar el token final.
*/

static int	generate_token_list(const char *input, t_token **head,
			t_token **current)
{
	int		i;
	t_token	*new_token;

	i = 0;
	while (input[i])
	{
		while (input[i] && input[i] == ' ')
			i++;
		if (!input[i])
			break ;
		if (is_operator_char(input[i]))
			new_token = process_operator(input, &i);
		else
			new_token = process_word(input, &i);
		if (!new_token)
			return (1);
		add_token(head, current, new_token);
	}
	return (0);
}
/*
 * Propósito: Es el motor principal del tokenizador. Recorre la cadena de
 *            entrada y construye la lista de tokens.
 * Mecanismo:
 *   1. Itera sobre la cadena de entrada `input` con un bucle `while`.
 *   2. En cada iteración, primero salta todos los espacios y tabulaciones.
 *   3. Comprueba si el carácter actual es un operador. Si lo es, llama a
 *      `process_operator`. Si no, llama a `process_word`.
 *   4. El token resultante se añade al final de la lista enlazada a través
 *      de `add_token`.
 *   5. Si en algún momento la creación de un token falla, retorna 1 (error).
 * Llamado por: `tokenize`, para realizar la tokenización principal.
 * Llama a:
 *   - `is_operator_char`: Para decidir qué tipo de token procesar.
 *   - `process_operator`: Para manejar tokens de operador.
 *   - `process_word`: Para manejar tokens de palabra.
 *   - `add_token`: Para añadir el nuevo token a la lista.
*/

t_token	*tokenize(const char *input)
{
	t_token	*head;
	t_token	*current;
	t_token	*new_token;

	if (!input)
		return (NULL);
	head = NULL;
	current = NULL;
	if (generate_token_list(input, &head, &current) != 0)
		return (cleanup_tokens(head), NULL);
	new_token = create_token(TOKEN_EOF, NULL);
	if (!new_token)
		return (cleanup_tokens(head), NULL);
	add_token(&head, &current, new_token);
	return (head);
}
/*
 * Propósito: Es la función pública y punto de entrada para la tokenización.
 *            Orquesta todo el proceso desde la entrada hasta la salida.
 * Mecanismo:
 *   1. Inicializa los punteros `head` y `current` de la lista a NULL.
 *   2. Llama a `generate_token_list` para que construya la lista de tokens.
 *   3. Si `generate_token_list` retorna un error, limpia la lista
 *      parcialmente creada con `cleanup_tokens` y retorna NULL.
 *   4. Si tiene éxito, crea y añade un token final `TOKEN_EOF` para marcar
 *      el final de la secuencia, lo cual es crucial para el parser.
 *   5. Devuelve el puntero al inicio de la lista (`head`).
 * Llamado por: `main`, es el primer paso después de recibir la entrada del
 *              usuario.
 * Llama a:
 *   - `generate_token_list`: Para hacer el trabajo pesado de la tokenización.
 *   - `create_token`: Para crear el token final `EOF`.
 *   - `add_token`: Para añadir el token `EOF` a la lista.
 *   - `cleanup_tokens`: En caso de error para prevenir fugas de memoria.
*/
