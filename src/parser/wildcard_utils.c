#include "../../include/minishell.h"

// Función recursiva para comparar una cadena con un patrón.
// str: el nombre del fichero (ej: "minishell.c")
// pattern: el patrón con wildcard (ej: "*.c")
int	match_wildcard(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '*')
	{
		if (*(pattern + 1) == '\0')
			return (1); // '*' al final coincide con todo lo que queda
		while (*str)
		{
			if (match_wildcard(str, pattern + 1))
				return (1);
			str++;
		}
		return (match_wildcard(str, pattern + 1));
	}
	if (*str == *pattern)
		return (match_wildcard(str + 1, pattern + 1));
	return (0);
}
