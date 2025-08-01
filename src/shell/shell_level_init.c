#include "../../include/minishell.h"

static void	levelup_shell(int *shlvl_num, char *current_shlvl_str)
{
	*shlvl_num = ft_atoi(current_shlvl_str);
	if (*shlvl_num >= 999)
	{
		ft_putendl_fd("minishell: warning: shell level ", 2);
		ft_putendl_fd("(1000) too high, resetting to 1", 2);
	}
	if (*shlvl_num >= 999)
		*shlvl_num = 1;
	else
		(*shlvl_num)++;
}

void	update_shell_level(t_shell_context *shell_context)
{
	char	*current_shlvl_str;
	int		shlvl_num;
	char	*new_shlvl_str;
	char	*export_arg;
	char	*export_args[3];

	current_shlvl_str = get_env_value("SHLVL", shell_context);
	shlvl_num = 1;
	if (current_shlvl_str && ft_isnumstr(current_shlvl_str))
		levelup_shell(&shlvl_num, current_shlvl_str);
	new_shlvl_str = ft_itoa(shlvl_num);
	export_arg = ft_strjoin("SHLVL=", new_shlvl_str);
	free(new_shlvl_str);
	export_args[0] = "export";
	export_args[1] = export_arg;
	export_args[2] = NULL;
	builtin_export(export_args, shell_context);
	free(export_arg);
}
