#include "../include/minishell.h"

static void	print_node_args(const t_ast_node *node)
{
	int	i;

	i = 0;
	printf("  args: ");
	while (node->args[i])
	{
		printf("[%s] ", node->args[i]);
		i++;
	}
	printf("\n");
}

void	debug_print_ast(const t_ast_node *node)
{
	printf("DEBUG: AST nodes:\n");
	if (!node)
	{
		printf("AST: (null)\n");
		return ;
	}
	printf("AST Node: type=%d\n", node->type);
	if (node->args)
		print_node_args(node);
	if (node->file)
		printf("  file: [%s]\n", node->file);
	if (node->left)
	{
		printf("  left:\n");
		debug_print_ast(node->left);
	}
	if (node->right)
	{
		printf("  right:\n");
		debug_print_ast(node->right);
	}
}
