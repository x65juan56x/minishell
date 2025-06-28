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

static int	setup_redirect_and_get_fd(t_ast_node *node)
{
	if (node->type == NODE_HEREDOC)
		return (execute_heredoc(node->file));
	return (open_redirect_file(node->file, node->type));
}

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

/* Recolecta en 'arr' las redirecciones encadenadas en 'node'; devuelve el count */
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

/* Aplica N redirects LIFO y ejecuta el comando */
int	execute_redirect_node(t_ast_node *node, char **envp)
{
	t_ast_node	*reds[64];
	t_ast_node	*cmd;
	int			count;
	pid_t		pid;
	int			status;

	if (!node || !node->file)
		return (1);
	count = collect_redirect_nodes(node, reds);	/* Recolectar nodos de redirección en orden inverso */
	cmd = reds[--count]->left;	/* El comando es lo que queda tras las redirects */
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		while (count >= 0)
			apply_single_redirect(reds[count--]);
		exit(execute_ast(cmd, envp));
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
