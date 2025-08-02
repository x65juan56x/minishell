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
