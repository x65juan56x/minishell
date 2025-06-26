#include "../../include/minishell.h"

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
	{
		if (value)
			free(value);
		return (NULL);
	}
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}
/*
 * Propósito: Crear y alocar memoria para un único nodo de token.
 * Mecanismo:
 *   1. Usa `malloc` para reservar espacio para la estructura `t_token`.
 *   2. Si `malloc` falla, se asegura de liberar el `value` (si existe) que
 *      se le pasó, para evitar fugas de memoria.
 *   3. Asigna el `type` y el `value` a la nueva estructura.
 *   4. Inicializa el puntero `next` a NULL.
 * Llamado por: `process_operator`, `process_word` y `tokenize`. Es la fábrica
 *              central de todos los tokens.
 * Llama a: `malloc`, `free`.
*/

void	add_token(t_token **head, t_token **current, t_token *new_token)
{
	if (!*head)
	{
		*head = new_token;
		*current = new_token;
	}
	else
	{
		(*current)->next = new_token;
		*current = new_token;
	}
}
/*
 * Propósito: Añadir un nuevo token al final de la lista enlazada.
 * Mecanismo:
 *   - Usa punteros dobles (`**head`, `**current`) para poder modificar los
 *     punteros originales en la función que la llama.
 *   - Si la lista está vacía (`!*head`), el nuevo token se convierte tanto en
 *     la cabeza (`head`) como en el token actual (`current`).
 *   - Si la lista ya tiene nodos, enlaza el `next` del token actual al nuevo
 *     token y luego actualiza el puntero `current` para que apunte al nuevo
 *     final de la lista.
 * Llamado por: `generate_token_list` y `tokenize`.
*/

void	cleanup_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}
/*
 * Propósito: Liberar toda la memoria utilizada por una lista enlazada de tokens.
 * Mecanismo:
 *   1. Itera a través de la lista enlazada desde la cabeza (`tokens`).
 *   2. En cada paso, guarda un puntero al siguiente nodo (`next`) antes de
 *      liberar el nodo actual. Esto es crucial para no perder la referencia
 *      al resto de la lista.
 *   3. Liberar el nodo actual y su contenido.
 *   4. Avanza al siguiente nodo.
 * Llamado por: `main` (en el flujo normal) y por `tokenize` (en caso de error).
 *              Es la función principal de limpieza para el tokenizador.
 * Llama a: `free`.
*/
