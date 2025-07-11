#include "../../include/minishell.h"

int	handle_word_token(t_token **token, char **args, int *i)
{
	t_token	*temp;

	temp = *token;
	args[*i] = ft_strdup(temp->value);
	if (!args[*i])
		return (-1);
	(*i)++;
	*token = temp->next;
	return (0);
}
/*
 * Propósito: Procesar un único token de tipo `TOKEN_WORD` durante la extracción
 *            de argumentos.
 * Mecanismo:
 *   1. Duplica el valor del token (`temp->value`) y lo asigna a la posición
 *      actual del array `args`.
 *   2. Si la duplicación falla, retorna -1 para señalar un error de memoria.
 *   3. Incrementa el índice `i` del array de argumentos.
 *   4. Avanza el puntero de token `token` al siguiente token de la lista.
 * Llamado por: `extract_args`.
 * Llama a: `ft_strdup`.
*/

void	skip_redirect_token(t_token **token)
{
	t_token	*temp_token;

	temp_token = *token;
	temp_token = temp_token->next;
	if (temp_token && temp_token->type == TOKEN_WORD)
		temp_token = temp_token->next;
	*token = temp_token;
}
/*
 * Propósito: Saltar un par de tokens que representan una redirección
 *            (operador + nombre de archivo).
 * Mecanismo:
 *   1. Avanza el puntero de token `token` para saltar el operador (ej: '>').
 *   2. Vuelve a avanzar `token` para saltar el nombre del archivo asociado.
 * Llamado por: `extract_args`.
*/

int	extract_args(char **args, int max, t_token **token)
{
	int		i;
	t_token	*temp_token;

	i = 0;
	temp_token = *token;
	while (i < max && temp_token && temp_token->type != TOKEN_PIPE && temp_token->type != TOKEN_EOF)
	{
		if (temp_token->type == TOKEN_WORD)
		{
			if (handle_word_token(&temp_token, args, &i) < 0)
				return (-1);
			continue ;
		}
		if (is_redirect_token(temp_token->type))
		{
			skip_redirect_token(&temp_token);
			continue ;
		}
		break ;
	}
	*token = temp_token;
	return (i);
}
/*
 * Propósito: Llenar un array `args` con un máximo de `max` argumentos,
 *            extrayéndolos de la lista de tokens y saltando las redirecciones.
 * Mecanismo:
 *   1. Itera sobre los tokens mientras no se alcance el máximo de argumentos
 *      y no se encuentre un pipe o el final de la lista.
 *   2. Si el token es `TOKEN_WORD`, llama a `handle_word_token` para añadirlo
 *      al array.
 *   3. Si el token es de redirección, llama a `skip_redirect_token` para
 *      ignorarlo a él y a su archivo.
 *   4. Si es cualquier otro token, detiene la extracción.
 *   5. Actualiza el puntero `token` para que apunte al primer token no consumido.
 *   6. Devuelve el número de argumentos extraídos.
 * Llamado por: `collect_command_args`.
 * Llama a:
 *   - `handle_word_token`: Para procesar palabras.
 *   - `skip_redirect_token`: Para ignorar redirecciones.
 *   - `is_redirect_token`: Para identificar operadores de redirección.
*/

t_token	*consume_token_type(t_parser *parser, t_token_type type)
{
	t_token	*temp;

	if (!parser->current || parser->current->type != type)
	{
		parser->error = 1;
		return (NULL);
	}
	temp = parser->current;
	parser->current = temp->next;
	return (temp);
}
/*
 * Propósito: Consumir el token actual si y solo si coincide con un tipo
 *            específico. Es una versión más estricta de `consume_token`.
 * Mecanismo:
 *   1. Comprueba si el token actual existe y si su tipo es el esperado (`type`).
 *   2. Si no coincide, activa el flag de error del parser y retorna NULL.
 *   3. Si coincide, guarda el token, avanza el parser y lo retorna.
 * Llamado por: `parse_pipe_expression`, para consumir específicamente el
 *              token de `|`.
*/

int	are_quotes_unclosed(const char *s)
{
	char	in_quote; // 0 = no, '\'' = en comilla simple, '"' = en comilla doble

	in_quote = 0;
	while (*s)
	{
		if (*s == '\'' && in_quote == 0)
			in_quote = '\'';
		else if (*s == '"' && in_quote == 0)
			in_quote = '"';
		else if (*s == in_quote)
			in_quote = 0; // Se cierra la comilla
		s++;
	}
	return (in_quote);
}
