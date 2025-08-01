#include "../include/minishell.h"

void	debug_print_local_vars(t_shell_context *ctx)
{
	t_list	*cur;

	cur = ctx->local_vars;
	printf("DEBUG: local_vars: ");
	while (cur)
	{
		printf("[%s] ", (char *)cur->content);
		cur = cur->next;
	}
	printf("\n");
}

void	debug_print_token_list(t_token *head)
{
	t_token	*cur;

	cur = head;
	printf("DEBUG: token list: ");
	while (cur)
	{
		printf("[%s] ", (char *)cur->value);
		cur = cur->next;
	}
	printf("\n");
}

void	debug_print_parser(const t_parser *parser)
{
	if (!parser)
	{
		printf("DEBUG: parser is NULL\n");
		return ;
	}
	printf("DEBUG: t_parser\n");
	printf("  error: %d\n", parser->error);
	printf("  tokens: ");
	debug_print_token_list(parser->tokens);
	printf("  current token: ");
	if (parser->current)
		printf("[%s] (type=%d)\n",
			parser->current->value, parser->current->type);
	else
		printf("(null)\n");
}

void	debug_print_expand(t_token token)
{
	printf("DEBUG: expand: ");
	printf("[%i] ", token.expand);
	printf("\n");
}
