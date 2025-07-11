#include "../../include/minishell.h"

static int	open_redirect_file(char *file, t_node_type type)
{
	int	fd;

	if (type == NODE_REDIRECT_IN)
		fd = open(file, O_RDONLY);
	else if (type == NODE_REDIRECT_OUT)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (type == NODE_REDIRECT_APPEND)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return (-1);
	if (fd < 0)
		perror(file);
	return (fd);
}
/*
 * Propósito: Abrir el archivo adecuado según el tipo de redirección.
 * Mecanismo:
 *   - `<`  abre en O_RDONLY.
 *   - `>`  abre/crea en O_WRONLY|O_CREAT|O_TRUNC.
 *   - `>>` abre/crea en O_WRONLY|O_CREAT|O_APPEND.
 *   1. Llama a `open` con flags según `type`.
 *   2. Si falla, llama a `perror(file)`.
 *   3. Devuelve el descriptor o -1.
 * Llamado por: `setup_redirect_and_get_fd`.
 * Llama a:
 *   - `open`
 *   - `perror`
 */

static int	setup_redirect_and_get_fd(t_ast_node *node)
{
	if (node->type == NODE_HEREDOC)
		return (execute_heredoc(node->file));
	return (open_redirect_file(node->file, node->type));
}
/*
 * Propósito: Obtener el fd que se usará para la redirección.
 * Mecanismo:
 *   1. Si `node->type == NODE_HEREDOC`, invoca `execute_heredoc`.
 *   2. En otro caso, invoca `open_redirect_file`.
 * Llamado por: `apply_single_redirect`.
 * Llama a:
 *   - `execute_heredoc`
 *   - `open_redirect_file`
 */

static int	apply_single_redirect(t_ast_node *r)
{
	int	fd;
	int	target;

	fd = setup_redirect_and_get_fd(r);
	if (fd < 0)
		return (1);
	if (r->type == NODE_REDIRECT_IN || r->type == NODE_HEREDOC)
		target = STDIN_FILENO;
	else
		target = STDOUT_FILENO;
	if (dup2(fd, target) < 0)
	{
		perror("dup2");
		close(fd);
		exit(1);
	}
	close(fd);
	return (0);
}
/*
 * Propósito: Redirigir STDIN/STDOUT al archivo o pipe correspondiente.
 * Mecanismo:
 *   1. Obtiene fd con `setup_redirect_and_get_fd`.
 *   2. Si fd < 0, retorna 1 (error).
 *   3. Determina `target` según tipo de redirección.
 *   4. Llama a `dup2(fd, target)` y comprueba errores.
 *   5. Cierra el fd original.
 * Llamado por: `execute_redirect_node`.
 * Llama a:
 *   - `setup_redirect_and_get_fd`
 *   - `dup2`
 *   - `perror`
 *   - `close`
 */

static int	collect_redirect_nodes(t_ast_node *node, t_ast_node *arr[])
{
	int			count;
	t_ast_node	*cur;

	count = 0;
	cur = node;
	while (cur && cur->type >= NODE_REDIRECT_IN && cur->type <= NODE_HEREDOC)
	{
		arr[count++] = cur;
		cur = cur->left;
	}
	return (count);
}
/*
 * Propósito: Recolectar nodos de redirección encadenados en un array.
 * Mecanismo:
 *   1. Recorre la cadena `node->left->left->…` mientras sean redirects.
 *   2. Guarda cada nodo en `arr[count++]`.
 *   3. Devuelve el número total de redirects encontrados.
 * Llamado por: `execute_redirect_node`.
 */

int	execute_redirect_node(t_ast_node *node, char ***envp_ptr)
{
	t_ast_node	*reds[64];
	t_ast_node	*cmd;
	int			count;
	pid_t		pid;
	int			status;

	if (!node || !node->file)
		return (1);
	count = collect_redirect_nodes(node, reds);
	cmd = reds[--count]->left;
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		setup_child_signals();
		while (count >= 0)
		{
			if (apply_single_redirect(reds[count--]) == 1)
				exit(1);
		}
		exit(execute_ast(cmd, envp_ptr));
	}
	ignore_signals();
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		return (128 + WTERMSIG(status));
	}
	return (1);
}
/*
 * Propósito: Ejecutar un comando con todas sus redirecciones.
 * Mecanismo:
 *   1. Recolecta nodos de redirect en orden inverso con `collect_redirect_nodes`.
 *   2. Obtiene el nodo de comando original (`cmd`).
 *   3. Crea un hijo con `fork()`.
 *   4. En el hijo:
 *      a. Aplica cada redirect con `apply_single_redirect` (LIFO).
 *      b. Llama a `execute_ast(cmd, envp)` y sale.
 *   5. En el padre, espera al hijo con `waitpid`.
 *   6. Devuelve el código de salida o 128+ señal.
 * Llamado por: `execute_ast` cuando `ast->type` es redirect.
 * Llama a:
 *   - `collect_redirect_nodes`
 *   - `fork`
 *   - `apply_single_redirect`
 *   - `execute_ast`
 *   - `waitpid`
 */
