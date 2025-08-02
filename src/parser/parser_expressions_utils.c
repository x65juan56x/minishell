#include "../../include/minishell.h"

char	**convert_arg_list_to_array(t_list *arg_list)
{
	char	**args;
	int		size;
	int		i;
	t_list	*current;

	size = ft_lstsize(arg_list);
	args = malloc(sizeof(char *) * (size + 1));
	if (!args)
		return (NULL);
	i = 0;
	current = arg_list;
	while (current)
	{
		args[i] = ft_strdup((char *)current->content);
		if (!args[i])
		{
			while (--i >= 0)
				free(args[i]);
			return (free(args), NULL);
		}
		current = current->next;
		i++;
	}
	args[i] = NULL;
	return (args);
}

static int	process_word_token(t_parser *parser, t_list **arg_list)
{
	char	*word_copy;
	t_list	*new_node;

	word_copy = ft_strdup(parser->current->value);
	if (!word_copy)
		return (-1);
	new_node = ft_lstnew(word_copy);
	if (!new_node)
	{
		free(word_copy);
		return (-1);
	}
	ft_lstadd_back(arg_list, new_node);
	parser->current = parser->current->next;
	return (0);
}

static int	process_redirect_token(t_parser *parser, t_ast_node **node)
{
	t_token		*op;
	t_token		*file;
	t_ast_node	*new_redirect_node;

	op = consume_token(parser, parser->current->type);
	if (!op)
		return (-1);
	file = consume_token(parser, TOKEN_WORD);
	if (!file)
	{
		parser->error = 1;
		return (-1);
	}
	new_redirect_node = create_redirect_node(op->type, *node, file->value);
	if (!new_redirect_node)
		return (-1);
	*node = new_redirect_node;
	return (0);
}

int	process_tokens_loop(t_parser *parser, t_list **arg_list,
				t_ast_node **node)
{
	while (parser->current && (parser->current->type == TOKEN_WORD
			|| is_redirect_token(parser->current->type)))
	{
		if (parser->current->type == TOKEN_WORD)
		{
			if (process_word_token(parser, arg_list) != 0)
				return (-1);
		}
		else if (process_redirect_token(parser, node) != 0)
			return (-1);
	}
	return (0);
}
