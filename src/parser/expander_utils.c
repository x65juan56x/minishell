#include "../../include/minishell.h"

char	*expand_pid(int *i)
{
	pid_t	pid;
	char	*pid_str;

	pid = getpid();
	pid_str = ft_itoa(pid);
	*i = *i + 2;
	return (pid_str);
}

char	*expand_status(int *i, t_shell_context *shell_context)
{
	int		status;
	char	*status_str;
	int		len;

	status = shell_context->exit_status;
	status_str = ft_itoa(status);
	len = ft_strlen(status_str);
	*i = *i + len;
	return (status_str);
}

char	*extract_var_name(char *str, int *i)
{
	int	len;

	len = 0;
	while (str[(*i) + len] && (ft_isalnum(str[(*i) + len])
			|| str[(*i) + len] == '_'))
		len++;
	return (ft_substr(str, *i, len));
}

char	*expand_var(int *i, t_shell_context *shell_context, t_token *token)
{
	char	*env_value;
	char	*variable;

	variable = extract_var_name(token->value, i);
	*i = *i + ft_strlen(variable);
	env_value = get_env_value(variable, shell_context);
	free(variable);
	if (env_value)
		return (ft_strdup(env_value));
	return (ft_strdup(""));
}

char	*expand_curly(int *i, t_shell_context *shell_context, t_token *token)
{
	char	*env_value;

	(*i)++;
	env_value = expand_var(i, shell_context, token);
	if (token->value[*i] != '}')
		return (ft_strdup(""));
	else
		(*i)++;
	return (env_value);
}
