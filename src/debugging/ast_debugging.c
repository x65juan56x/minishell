#include "../include/minishell.h"

static void	print_node_args(const t_ast_node *node)
{
	int	i;

	i = 0;
	printf("  args: ");
	if (!node->args)
	{
		printf("(NULL)\n");
		return ;
	}
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
	printf("AST Node: type=%d, node_ptr=%p\n", node->type, node);
	if (node->args)
		print_node_args(node);
	if (node->file)
		printf("  file: [%s]\n", node->file);
	if (node->left)
	{
		printf("  left: %p\n", node->left);
		debug_print_ast(node->left);
	}
	if (node->right)
	{
		printf("  right: %p\n", node->right);
		debug_print_ast(node->right);
	}
}
