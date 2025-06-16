#include "../../include/minishell.h"

static void	print_indent(int depth)
{
	int	i;

	i = 0;
	while (i < depth)
	{
		printf("  ");
		i++;
	}
}

static void	print_node_type(t_node_type type)
{
	if (type == NODE_COMMAND)
		printf("COMMAND");
	else if (type == NODE_PIPE)
		printf("PIPE");
	else if (type == NODE_REDIRECT_IN)
		printf("REDIRECT_IN");
	else if (type == NODE_REDIRECT_OUT)
		printf("REDIRECT_OUT");
	else if (type == NODE_REDIRECT_APPEND)
		printf("REDIRECT_APPEND");
	else if (type == NODE_HEREDOC)
		printf("HEREDOC");
	else if (type == NODE_AND)
		printf("AND");
	else if (type == NODE_OR)
		printf("OR");
	else
		printf("UNKNOWN");
}

static void	print_command_args(char **args)
{
	int	i;

	if (!args)
	{
		printf(" [NO ARGS]");
		return ;
	}
	printf(" [");
	i = 0;
	while (args[i])
	{
		printf("\"%s\"", args[i]);
		if (args[i + 1])
			printf(", ");
		i++;
	}
	printf("]");
}

void	print_ast(t_ast_node *node, int depth)
{
	if (!node)
	{
		print_indent(depth);
		printf("(NULL)\n");
		return ;
	}
	print_indent(depth);
	print_node_type(node->type);
	if (node->type == NODE_COMMAND)
		print_command_args(node->args);
	else if (node->file)
		printf(" file: \"%s\"", node->file);
	printf("\n");
	if (node->left)
	{
		print_indent(depth);
		printf("├─ left:\n");
		print_ast(node->left, depth + 1);
	}
	if (node->right)
	{
		print_indent(depth);
		printf("└─ right:\n");
		print_ast(node->right, depth + 1);
	}
}
