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
	while (i < max && temp_token && temp_token->type == TOKEN_WORD)
		if (handle_word_token(&temp_token, args, &i) < 0)
			return (-1);
	*token = temp_token;
	return (i);
}

t_token	*consume_token_type(t_parser *parser, t_token_type type)
{
    t_token	*temp;

    if (!parser->current || parser->current->type != type)
    {
        if (!parser->error) // Imprimir solo el primer error para evitar cascadas
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
            if (parser->current && parser->current->type != TOKEN_EOF)
                ft_putstr_fd(parser->current->value, STDERR_FILENO);
            else
                ft_putstr_fd("newline", STDERR_FILENO); // Error al final de la línea
            ft_putstr_fd("'\n", STDERR_FILENO);
        }
        parser->error = 1;
        return (NULL);
    }
    temp = parser->current;
    parser->current = temp->next;
    return (temp);
}

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

// int	is_lparen_and_or_open(const char *s)
// {
// 	int	i;

// 	i = 0;
// 	while (s[i])
// 	{
// 		if (s[i + 1] == '&' && s[i + 2] == '&' && !s[i + 3])
// 			return (1);
// 		if (s[i + 1] == '|' && s[i + 2] == '|' && !s[i + 3])
// 			return (1);
// 		if (s[i] == '(' && !s[i + 1])
// 			return (1);
// 		i++;
// 	}
// 	return (0);
// }

int	are_parentheses_unclosed(const char *s)
{
    int		i;
    int		paren_level;
    char	in_quote;

    i = 0;
    paren_level = 0;
    in_quote = 0;
    while (s[i])
    {
        if (s[i] == '\'' && in_quote == 0)
            in_quote = '\'';
        else if (s[i] == '"' && in_quote == 0)
            in_quote = '"';
        else if (s[i] == in_quote)
            in_quote = 0;
        else if (s[i] == '(' && in_quote == 0)
            paren_level++;
        else if (s[i] == ')' && in_quote == 0)
            paren_level--;
        i++;
    }
    return (paren_level > 0);
}