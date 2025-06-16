#include "../../include/minishell.h"

static int	count_command_args(t_parser *parser)
{
    t_token	*temp;
    int		count;

    count = 0;
    temp = parser->current;
    while (temp && temp->type == TOKEN_WORD)
    {
        count++;
        temp = temp->next;
    }
    return (count);
}

t_token	*consume_token(t_parser *parser, t_token_type expected)
{
    t_token	*token;

    if (!parser->current || parser->current->type != expected)
    {
        parser->error = 1;
        return (NULL);
    }
    token = parser->current;
    parser->current = parser->current->next;
    return (token);
}

char	**collect_command_args(t_parser *parser)
{
    char	**args;
    int		count;
    int		i;
    t_token	*token;

    count = count_command_args(parser);
    if (count == 0)
        return (NULL);
    args = malloc(sizeof(char *) * (count + 1));
    if (!args)
        return (NULL);
    i = 0;
    while (i < count)
    {
        token = consume_token(parser, TOKEN_WORD);
        if (!token)
            return (ft_freearr(args), NULL);
        args[i] = ft_strdup(token->value);
        if (!args[i])
            return (ft_freearr(args), NULL);
        i++;
    }
    args[i] = NULL;
    return (args);
}

t_ast_node	*parse_command(t_parser *parser)
{
    t_ast_node	*node;

    if (!parser->current || parser->current->type != TOKEN_WORD)
        return (NULL);
    node = create_ast_node(NODE_COMMAND);
    if (!node)
        return (NULL);
    node->args = collect_command_args(parser);
    if (!node->args)
    {
        free(node);
        parser->error = 1;
        return (NULL);
    }
    return (node);
}
