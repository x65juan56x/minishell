#include "../../include/minishell.h"

static void	setup_pipe_child(int *pipe_fd, int is_left_child)
{
	if (is_left_child)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			close(pipe_fd[1]);
			exit(1);
		}
		close(pipe_fd[1]);
	}
	else
	{
		close(pipe_fd[1]);
		if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
		{
			close(pipe_fd[0]);
			exit(1);
		}
		close(pipe_fd[0]);
	}
}
/*
 * Propósito: Configurar la entrada/salida de un proceso hijo para pipes.
 * Mecanismo:
 *   - Si es hijo izquierdo (`is_left_child`):
 *     1. Cierra lectura (`fd[0]`).
 *     2. Duplica escritura a STDOUT.
 *     3. Cierra la copia original.
 *   - Si es hijo derecho:
 *     1. Cierra escritura (`fd[1]`).
 *     2. Duplica lectura a STDIN.
 *     3. Cierra la copia original.
 * Llamado por: `create_pipe_child`.
 * Llama a:
 *   - `close`
 *   - `dup2`
 *   - `exit`
 */

static void	setup_heredoc_input(int heredoc_fd)
{
	printf("[pipe_executor] setup_heredoc_input: heredoc_fd=%d\n", heredoc_fd);
	if (heredoc_fd == -1)
		return ;
	if (dup2(heredoc_fd, STDIN_FILENO) == -1)
		exit(1);
	close(heredoc_fd);
}
/*
 * Propósito: Enlazar el read-end de here-doc al STDIN antes del pipe.
 * Mecanismo:
 *   1. Si `heredoc_fd` >= 0, duplica a STDIN.
 *   2. Cierra `heredoc_fd`.
 * Llamado por: `create_pipe_child` cuando hay here-doc en el pipeline.
 * Llama a:
 *   - `dup2`
 *   - `close`
 *   - `exit`
 */

static void	child_process_routine(t_ast_node *node, char **envp)
{
	printf("[pipe_executor] child_process_routine: tipo nodo=%d\n", node->type);
	if (node->type == NODE_COMMAND)
	{
		if (node->args && node->args[0])
			launch_command(node->args, envp);
	}
	else
		execute_ast(node, envp);
	exit(1);
}
/*
 * Propósito: Ejecutar el nodo AST dentro del proceso hijo del pipe.
 * Mecanismo:
 *   1. Si `node->type == NODE_COMMAND`, llama a `launch_command`.
 *   2. Si es otro tipo (pipe o redirect), llama recursivamente a `execute_ast`.
 *   3. Siempre finaliza con `exit(1)` para seguridad.
 * Llamado por: `create_pipe_child` en el contexto del hijo.
 * Llama a:
 *   - `launch_command`
 *   - `execute_ast`
 *   - `exit`
 */

pid_t	create_pipe_child(t_ast_node *node, t_pipe_config *config)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		if (config->is_left)
			close(config->pipe_fd[1]);
		else
			close(config->pipe_fd[0]);
		if (config->heredoc_fd != -1)
			close(config->heredoc_fd);
		return (-1);
	}
	if (pid == 0)
	{
		printf("[pipe_executor] HIJO %s: pid=%d, heredoc_fd=%d\n", config->is_left ? "IZQUIERDO" : "DERECHO", getpid(), config->heredoc_fd);
		if (config->is_left)
			setup_heredoc_input(config->heredoc_fd);
		setup_pipe_child(config->pipe_fd, config->is_left);
		child_process_routine(node, config->envp);
	}
	else
	{
		printf("[pipe_executor] PADRE: creado hijo %s pid=%d\n",
			config->is_left ? "IZQUIERDO" : "DERECHO", pid);
	}
	return (pid);
}
/*
 * Propósito: Crear un proceso hijo para un extremo del pipe.
 * Mecanismo:
 *   1. `fork()`.
 *   2. Si falla, cierra los fds asociados y devuelve -1.
 *   3. En el hijo:
 *      a. Si es extremo izquierdo, aplica `setup_heredoc_input`.
 *      b. Llama a `setup_pipe_child` para redirigir STDIN/STDOUT.
 *      c. Ejecuta `child_process_routine`.
 * Llamado por: `create_pipe_and_execute` en `executor.c`.
 * Llama a:
 *   - `fork`
 *   - `close`
 *   - `setup_heredoc_input`
 *   - `setup_pipe_child`
 *   - `child_process_routine`
 */

int	wait_pipe_children(pid_t left_pid, pid_t right_pid)
{
	int	left_status;
	int	right_status;

	if (left_pid > 0)
		waitpid(left_pid, &left_status, 0);
	if (right_pid > 0)
		waitpid(right_pid, &right_status, 0);
	if (right_pid > 0 && WIFEXITED(right_status))
		return (WEXITSTATUS(right_status));
	if (right_pid > 0 && WIFSIGNALED(right_status))
		return (128 + WTERMSIG(right_status));
	if (left_pid > 0 && WIFEXITED(left_status))
		return (WEXITSTATUS(left_status));
	if (left_pid > 0 && WIFSIGNALED(left_status))
		return (128 + WTERMSIG(left_status));
	return (1);
}
/*
 * Propósito: Esperar a los dos hijos del pipe y retornar su código.
 * Mecanismo:
 *   1. `waitpid` para `left_pid` y `right_pid`.
 *   2. Si el derecho terminó normalmente, devolver su status.
 *   3. Si fue por señal, devolver 128 + signo.
 *   4. Si no, aplicar mismo criterio al izquierdo.
 *   5. Devolver 1 por defecto.
 * Llamado por: `execute_ast` para pipelines.
 * Llama a:
 *   - `waitpid`
 *   - `WIFEXITED` / `WEXITSTATUS`
 *   - `WIFSIGNALED` / `WTERMSIG`
 */
