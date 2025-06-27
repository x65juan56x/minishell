#include "../../include/minishell.h"

t_ast_node	*parse(t_token *tokens)
{
	t_parser	parser;
	t_ast_node	*ast;

	if (!tokens)
		return (NULL);
	parser.tokens = tokens;
	parser.current = tokens;
	parser.error = 0;
	ast = parse_pipe_expression(&parser);
	if (parser.error)
		return (cleanup_ast(ast), NULL);
	return (ast);
}
/*
 * Propósito: Orquestar todo el proceso de parsing. Es el punto de entrada
 *            principal que convierte una lista de tokens en un Abstract
 *            Syntax Tree (AST).
 * Mecanismo:
 *   1. Inicializa una estructura `t_parser` para mantener el estado del
 *      análisis (token actual, flag de error).
 *   2. Llama a `parse_pipe_expression`, la función que maneja el operador de
 *      menor precedencia, iniciando así el descenso recursivo.
 *   3. Comprueba si el flag `parser.error` se activó durante el proceso.
 *   4. Si hubo un error, limpia el AST parcialmente construido y retorna NULL.
 *   5. Si todo fue exitoso, retorna la raíz del AST completo.
 * Llamado por: `main`, después de que el tokenizador ha generado la lista de
 *              tokens.
 * Llama a:
 *   - `parse_pipe_expression`: Para iniciar el análisis sintáctico.
 *   - `cleanup_ast`: Para liberar memoria en caso de error.
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
 * Propósito: Liberar de forma segura toda la memoria asociada a un AST.
 * Mecanismo:
 *   - Utiliza un recorrido en post-orden (primero hijos, luego el padre).
 *   - Llama recursivamente a `cleanup_ast` para el hijo izquierdo y derecho.
 *   - Una vez que los subárboles han sido liberados, libera el contenido del
 *     nodo actual (el array `args` y el string `file`).
 *   - Finalmente, libera la estructura del nodo en sí.
 * Llamado por:
 *   - `main`: Después de que el AST ha sido ejecutado, para la limpieza final.
 *   - `parse` y otras funciones del parser: En caso de error, para deshacer
 *     las alocaciones parciales y evitar fugas de memoria.
 * Llama a: `ft_freearr`, `free`, y a sí misma de forma recursiva.
*/
