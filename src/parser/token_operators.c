#include "../../include/minishell.h"

int	is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&' 
		|| c == '(' || c == ')');
}
/*
 * Propósito: Comprobar rápidamente si un carácter dado puede ser el inicio
 *            de un token de operador.
 * Mecanismo: Compara el carácter `c` con una lista predefinida de caracteres
 *            de operador.
 * Llamado por:
 *   - `generate_token_list`: Para decidir si debe llamar a `process_operator`
 *     o `process_word`.
 *   - `find_word_end`: Para saber cuándo debe terminar una palabra.
*/

static t_token_type	get_pipe_operator(const char *s, int *advance)
{
	if (s[1] == '|')
	{
		*advance = 2;
		return (TOKEN_OR);
	}
	*advance = 1;
	return (TOKEN_PIPE);
}
/*
 * Propósito: Determinar si un operador de pipe es simple (`|`) o doble (`||`).
 * Mecanismo:
 *   - Comprueba el segundo carácter (`s[1]`).
 *   - Si es `|`, es un `TOKEN_OR` y avanza 2 caracteres.
 *   - Si no, es un `TOKEN_PIPE` y avanza 1 carácter.
 *   - Actualiza el puntero `advance` para que el llamador sepa cuántos
 *     caracteres consumir.
 * Llamado por: `get_operator_type`, cuando el primer carácter es `|`.
*/

static t_token_type	get_redirect_operator(const char *s, int *advance)
{
	if (s[0] == '<')
	{
		if (s[1] == '<')
		{
			*advance = 2;
			return (TOKEN_HEREDOC);
		}
		*advance = 1;
		return (TOKEN_REDIRECT_IN);
	}
	if (s[1] == '>')
	{
		*advance = 2;
		return (TOKEN_REDIRECT_APPEND);
	}
	*advance = 1;
	return (TOKEN_REDIRECT_OUT);
}
/*
 * Propósito: Determinar el tipo exacto de operador de redirección.
 * Mecanismo:
 *   - Primero distingue entre `<` y `>`.
 *   - Luego, para cada uno, comprueba el segundo carácter para ver si es un
 *     operador doble (`<<` o `>>`).
 *   - Actualiza el puntero `advance` con la longitud del operador (1 o 2).
 * Llamado por: `get_operator_type`, cuando el primer carácter es `<` o `>`.
*/

t_token_type	get_operator_type(const char *s, int *advance)
{
	if (s[0] == '|')
		return (get_pipe_operator(s, advance));
	if (s[0] == '<' || s[0] == '>')
		return (get_redirect_operator(s, advance));
	if (s[0] == '&' && s[1] == '&')
	{
		*advance = 2;
		return (TOKEN_AND);
	}
	if (s[0] == '(')
	{
		*advance = 1;
		return (TOKEN_LPAREN);
	}
	if (s[0] == ')')
	{
		*advance = 1;
		return (TOKEN_RPAREN);
	}
	*advance = 1;
	return (TOKEN_WORD);
}
/*
 * Propósito: Es el dispatcher principal para identificar cualquier operador.
 * Mecanismo:
 *   - Usa una serie de `if` para comprobar el primer carácter (`s[0]`).
 *   - Delega a funciones más específicas (`get_pipe_operator`,
 *     `get_redirect_operator`) para los casos que pueden tener 1 o 2 caracteres.
 *   - Maneja los casos de 2 caracteres como `&&` directamente.
 *   - Maneja los casos de 1 carácter como `(` y `)` directamente.
 *   - Si no coincide con ningún operador conocido, por seguridad, lo trata como
 *     una palabra de 1 carácter (aunque la lógica en `generate_token_list`
 *     previene que esto ocurra con caracteres no-operador).
 * Llamado por: `process_operator`, es el cerebro de la identificación de
 *              operadores.
 * Llama a: `get_pipe_operator`, `get_redirect_operator`.
*/
