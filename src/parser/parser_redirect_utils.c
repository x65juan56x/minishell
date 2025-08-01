#include "../../include/minishell.h"

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_HEREDOC);
}

int	is_redirect_node(t_node_type type)
{
	return (type == NODE_REDIRECT_OUT
		|| type == NODE_REDIRECT_IN
		|| type == NODE_REDIRECT_APPEND
		|| type == NODE_HEREDOC);
}

int	parse_args_and_redirect(t_ast_node **node, char **args,
		int *arg_count, t_parser *parser)
{
	t_token	*op;
	t_token	*file;

	if (parser->current->type == TOKEN_WORD)
		args[(*arg_count)++] = ft_strdup(consume_token
				(parser, TOKEN_WORD)->value);
	else if (is_redirect_token(parser->current->type))
	{
		op = consume_token(parser, parser->current->type);
		file = consume_token(parser, TOKEN_WORD);
		if (!file)
		{
			parser->error = 1;
			ft_freearr(args);
			return (cleanup_ast(*node), -1);
		}
		*node = create_redirect_node(op->type, *node, file->value);
		if (!*node)
		{
			parser->error = 1;
			ft_freearr(args);
			return (-1);
		}
	}
	return (0);
}

char	**create_compact_args(char **args, int *arg_count)
{
	char	**compact_args;
	int		i;

	i = 0;
	compact_args = malloc(sizeof(char *) * (*arg_count + 2));
	if (!compact_args)
		return (NULL);
	if (arg_count == 0)
	{
		compact_args[0] = ft_strdup("");
		compact_args[1] = NULL;
		return (compact_args);
	}
	i = 0;
	while (i < *arg_count)
	{
		compact_args[i] = ft_strdup(args[i]);
		if (!compact_args[i])
			return (ft_freearr(compact_args), NULL);
		i++;
	}
	compact_args[*arg_count] = NULL;
	return (compact_args);
}
