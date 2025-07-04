#include "../../include/minishell.h"

static int	execute_command_node(t_ast_node *node, char ***envp_ptr)
{
	if (!node || !node->args || !node->args[0])
		return (0);
	if (is_builtin(node->args[0]))
		return (execute_builtin(node->args, envp_ptr));
	launch_command(node->args, *envp_ptr);
	return (127);
}
/*
 * Propósito: Ejecutar un comando simple en un proceso hijo.
 * Mecanismo:
 *   1. Si `node` o `node->args` no son válidos, retorna 0.
 *   2. Llama a `launch_command` (que nunca retorna si tiene éxito).
 *   3. Si retorna, propaga 127 (no ejecutable).
 * Llamado por: `execute_ast` cuando `ast->type == NODE_COMMAND`.
 * Llama a:
 *   - `launch_command`
 */

static int	create_pipe_and_execute(t_ast_node *node, char **envp,
			int heredoc_fd)
{
	int				pipe_fd[2];
	t_pipe_config	left_config;
	t_pipe_config	right_config;
	pid_t			left_pid;
	pid_t			right_pid;

	if (pipe(pipe_fd) == -1)
	{
		if (heredoc_fd != -1)
			close(heredoc_fd);
		return (1);
	}
	left_config = (t_pipe_config){pipe_fd, 1, heredoc_fd, envp};
	right_config = (t_pipe_config){pipe_fd, 0, -1, envp};
	left_pid = create_pipe_child(node->left, &left_config);
	right_pid = create_pipe_child(node->right, &right_config);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	if (heredoc_fd != -1)
		close(heredoc_fd);
	return (wait_pipe_children(left_pid, right_pid));
}
/*
 * Propósito: Ejecutar un nodo PIPE conectando dos hijos con tubería.
 * Mecanismo:
 *   1. Crea un pipe con `pipe()`.
 *   2. Configura `left_cfg` y `right_cfg` con fd y heredoc.
 *   3. Lanza dos hijos con `create_pipe_child`.
 *   4. Cierra ambos extremos del pipe en el padre.
 *   5. Cierra `heredoc_fd` si existe.
 *   6. Espera a los hijos y devuelve el status del derecho o izquierdo.
 * Llamado por: `execute_pipe_node` y `execute_ast`.
 * Llama a:
 *   - `pipe`
 *   - `create_pipe_child`
 *   - `wait_pipe_children`
 */

static int	execute_pipe_node(t_ast_node *node, char **envp)
{
	int	heredoc_fd;

	heredoc_fd = preprocess_heredocs(&(node->left)); // Pasar dirección
	return (create_pipe_and_execute(node, envp, heredoc_fd));
}
/*
 * Propósito: Ejecutar un nodo de pipe de alto nivel.
 * Mecanismo:
 *   1. Preprocesa here-docs en `node->left` con `preprocess_heredocs`.
 *   2. Llama a `create_pipe_and_execute` para forzar conectividad.
 * Llamado por: `execute_ast` cuando `ast->type == NODE_PIPE`.
 * Llama a:
 *   - `preprocess_heredocs`
 *   - `create_pipe_and_execute`
 */

int	execute_ast(t_ast_node *ast, char ***envp_ptr)
{
	pid_t	pid;
	int		status;

	if (!ast)
		return (0);
	if (ast->type == NODE_COMMAND)
	{
		if (is_builtin(ast->args[0])) // Si es built-in, ejecutar en el proceso padre
			return (execute_command_node(ast, envp_ptr));
		pid = fork(); // Si no es built-in, hacer fork
		if (pid == -1)
			return (perror("fork"), 1);
		if (pid == 0)
		{
			signals_default();
			execute_command_node(ast, envp_ptr);
			exit(127);
		}
		signal(SIGINT, SIG_IGN);
		waitpid(pid, &status, 0);
		signal(SIGINT, sigint_handler);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGINT)
				write(1, "\n", 1);
			else if (WTERMSIG(status) == SIGQUIT)
				write(1, "Quit (core dumped)\n", 20);
			return (128 + WTERMSIG(status));
		}
		return (1);
	}
	if (ast->type == NODE_PIPE)
		return (execute_pipe_node(ast, *envp_ptr));
	if (is_redirect_node(ast->type))
		return (execute_redirect_node(ast, *envp_ptr));
	return (1);
}
/*
 * Propósito: Punto único de ejecución del AST completo.
 * Mecanismo:
 *   1. Si `ast` es NULL, retorna 0 (nada que hacer).
 *   2. Si es `NODE_COMMAND`, crea un hijo y ejecuta comando.
 *   3. Si es `NODE_PIPE`, delega a `execute_pipe_node`.
 *   4. Si es nodo de redirección, delega a `execute_redirect_node`.
 *   5. Maneja códigos de salida y señales de cada caso.
 * Llamado por: `main` en el loop principal.
 * Llama a:
 *   - `fork`, `exit`, `waitpid`
 *   - `execute_command_node`
 *   - `execute_pipe_node`
 *   - `execute_redirect_node`
 *   - `is_redirect_node`
 */
