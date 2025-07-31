#include "../../include/minishell.h"

t_ast_node	*parse(t_token *tokens, t_shell_context *shell_context)
{
	t_parser	parser;
	t_ast_node	*ast;

	if (!tokens)
		return (NULL);
	parser.tokens = tokens;
	parser.current = tokens;
	parser.error = 0;
	ast = parse_logical_expression(&parser);
	if (parser.error)
	{
		if(tokens == NULL)
		{
			shell_context->error_flag = 1;
			cleanup_ast(ast);
			return(NULL);
		}	
	}
	if (parser.current->type != TOKEN_EOF)
	{
		consume_token_type(&parser, TOKEN_EOF); // Esto fallará a propósito y mostrará el error.
		cleanup_ast(ast);
		return (NULL);
	}
	return (ast);
}
// Usamos la función de error estándar para reportar el token inesperado.
/*
 * Propósito: Orquestar el proceso de parsing, convirtiendo una lista plana de
 *            tokens en un Árbol de Sintaxis Abstracta (AST) que representa la
 *            estructura jerárquica de los comandos.
 * Mecanismo:
 *   1. Inicializa una estructura `t_parser` que mantiene el estado del
 *      análisis (la lista de tokens, el token actual y un flag de error).
 *   2. Llama a `parse_pipe_expression`, la función que maneja el operador de
 *      menor precedencia ('|'), iniciando así el descenso recursivo.
 *   3. Al finalizar, comprueba si el flag `parser.error` se activó. Si es así,
 *      significa que hubo un error de sintaxis.
 *   4. En caso de error, libera la memoria del AST parcialmente construido.
 *   5. Si el parsing fue exitoso, devuelve la raíz del AST completo.
 * Llamado por: `main`, después de que el tokenizador ha generado la lista.
 * Llama a:
 *   - `parse_pipe_expression`: Para iniciar el análisis de la gramática.
 *   - `cleanup_ast`: Para liberar memoria en caso de error de sintaxis.
*/

void	cleanup_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->args)
		ft_freearr(node->args);
	if (node->file)
		free(node->file);
	cleanup_ast(node->left);
	cleanup_ast(node->right);
	free(node);
}
/*
 * Propósito: Liberar de forma segura toda la memoria asociada a un AST y sus
 *            subárboles para prevenir fugas de memoria.
 * Mecanismo:
 *   - Utiliza un recorrido en post-orden (hijos primero, luego el nodo padre).
 *   1. Si el nodo es NULL, no hay nada que hacer y retorna.
 *   2. Llama recursivamente a `cleanup_ast` para el subárbol izquierdo.
 *   3. Llama recursivamente a `cleanup_ast` para el subárbol derecho.
 *   4. Una vez que los hijos han sido liberados, libera los datos del nodo
 *      actual: el array de argumentos (`args`) y el nombre de archivo (`file`).
 *   5. Finalmente, libera la estructura del nodo en sí.
 * Llamado por:
 *   - `main`: Después de la ejecución, para la limpieza final.
 *   - `parse` y otras funciones del parser: En caso de error, para deshacer
 *     las alocaciones parciales.
 * Llama a:
 *   - `ft_freearr`: Para liberar el array de strings de argumentos.
 *   - `free`: Para liberar el nombre de archivo y el propio nodo.
 *   - `cleanup_ast` (a sí misma de forma recursiva).
*/
