#include "../../include/minishell.h"

static void	init_parser(t_parser *parser, t_token *tokens)
{
    parser->tokens = tokens;
    parser->current = tokens;
    parser->error = 0;
}
/* 
static t_token	*advance_token(t_parser *parser)
{
    t_token	*current;

    current = parser->current;
    if (parser->current && parser->current->type != TOKEN_EOF)
        parser->current = parser->current->next;
    return (current);
}
 */
static t_ast_node	*parse_expression(t_parser *parser)
{
    if (!parser->current || parser->error)
        return (NULL);
    return (parse_pipe_expression(parser));
}

t_ast_node	*parse(t_token *tokens)
{
    t_parser	parser;
    t_ast_node	*ast;

    if (!tokens)
        return (NULL);
    init_parser(&parser, tokens);
    ast = parse_expression(&parser);
    if (parser.error)
    {
        cleanup_ast(ast);
        return (NULL);
    }
    return (ast);
}

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
