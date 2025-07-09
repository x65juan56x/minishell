#include "../../include/minishell.h"

static int	find_word_end(const char *s, int start)
{
	int		i;
	char	quote;

	i = start;
	while (s[i] && !is_operator_char(s[i]) && s[i] != ' ')
	{
		if (s[i] == '\'' || s[i] == '"')
		{
			quote = s[i];
			i++;
			while (s[i] && s[i] != quote)
				i++;
			if (s[i] == quote)
				i++;
		}
		else
			i++;
	}
	return (i);
}
/*
 * Propósito: Encontrar el índice donde termina una "palabra" en la entrada.
 *            Una palabra puede contener comillas.
 * Mecanismo:
 *   1. Itera desde la posición `start`.
 *   2. El bucle se detiene si encuentra un espacio, un operador o el final
 *      de la cadena.
 *   3. Si encuentra una comilla (`'` o `"`), salta toda la sección
 *      entrecomillada de una vez, avanzando el índice hasta pasar la comilla
 *      de cierre. Esto asegura que los delimitadores dentro de las comillas
 *      no terminen la palabra.
 * Llamado por: `extract_word_token`, para delimitar la subcadena que
 *              compone el token de palabra.
 * Llama a: `is_operator_char`.
*/

char	*process_quoted_string(const char *s, int start, int end, int *quoted)
{
	char	*result;
	int		j;
	int		k;
	char	quote;

	result = malloc(end - start + 1);
	if (!result)
		return (NULL);
	j = 0;
	k = start;
	while (k < end)
	{
		if (s[k] == '\'' || s[k] == '"')
		{
			if(s[k] == '"')
				*quoted = 1;
			if(s[k] == '\'' )
				*quoted = 2;
			quote = s[k++];
			while (k < end && s[k] != quote)
				result[j++] = s[k++];
			if (k < end && s[k] == quote)
				k++;
		}
		else
			result[j++] = s[k++];
	}
	result[j] = '\0';
	return (result);
}
/*
 * Propósito: Crear la cadena de valor final para un token de palabra,
 *            eliminando las comillas externas que actúan como delimitadores.
 * Mecanismo:
 *   1. Reserva memoria para la nueva cadena.
 *   2. Itera a través de la subcadena original (de `start` a `end`).
 *   3. Si encuentra una comilla, copia el contenido interno de la sección
 *      entrecomillada, omitiendo las comillas delimitadoras.
 *   4. Si encuentra un carácter normal, lo copia directamente en el resultado.
 *   5. El resultado es una cadena "limpia", lista para ser usada por `execve`.
 *      Ej: `"hello world"` se convierte en `hello world`.
 * Llamado por: `extract_word_token`.
 * Llama a: `malloc`.
*/

char	*extract_word_token(const char *s, int *i, int *quoted)
{
	int	start;
	int	end;

	start = *i;
	end = find_word_end(s, start);
	*i = end;
	return (process_quoted_string(s, start, end, quoted));
}
/*
 * Propópisto: Orquestar la extracción completa de un token de palabra.
 * Mecanismo:
 *   1. Marca la posición de inicio.
 *   2. Llama a `find_word_end` para determinar dónde termina la palabra.
 *   3. Actualiza el índice principal `i` para que el tokenizador continúe
 *      desde la posición correcta en la siguiente iteración.
 *   4. Llama a `process_quoted_string` para obtener el valor final procesado
 *      (sin comillas externas).
 * Llamado por: `process_word`. Es la función de más alto nivel para el
 *              procesamiento de palabras.
 * Llama a: `find_word_end`, `process_quoted_string`.
*/
