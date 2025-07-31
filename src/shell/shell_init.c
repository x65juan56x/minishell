#include "../../include/minishell.h"

static char	**copy_envp(char **envp)
{
	char	**new_envp;
	int		i;
	int		count;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = -1;
	while (++i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (ft_freearr(new_envp), NULL);
	}
	new_envp[i] = NULL;
	return (new_envp);
}

static int	count_env_vars(char **envp)
{
	int	count;

	count = 0;
	if (!envp)
		return (0);
	while (envp[count])
		count++;
	return (count);
}

static void	print_empty_env_warning(void)
{
	ft_putstr_fd("minishell: warning: no environment", STDERR_FILENO);
	ft_putstr_fd(" variables detected\n", STDERR_FILENO);
	ft_putstr_fd("minishell: warning: shell behavior", STDERR_FILENO);
	ft_putstr_fd(" may be unexpected\n", STDERR_FILENO);
	ft_putstr_fd("minishell: warning: PATH not set ", STDERR_FILENO);
	ft_putstr_fd("- external commands will fail\n", STDERR_FILENO);
}

static int	has_essential_vars(char **envp)
{
	int	i;
	int	has_path;
	int	has_home;

	if (!envp)
		return (0);
	has_path = 0;
	has_home = 0;
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			has_path = 1;
		if (ft_strncmp(envp[i], "HOME=", 5) == 0)
			has_home = 1;
		i++;
	}
	return (has_path || has_home);
}

char	**init_shell_environment(char **envp, t_shell_context *shell_context)
{
	char	**envp_copy;
	int		env_count;

	env_count = count_env_vars(envp);
	
	if (env_count == 0 || !has_essential_vars(envp))
		print_empty_env_warning();
	
	envp_copy = copy_envp(envp);
	if (!envp_copy)
	{
		ft_putstr_fd("minishell: failed to initialize environment\n",
			STDERR_FILENO);
		cleanup_shell_context(shell_context);
		exit(1);
	}
	return (envp_copy);
}

static int	is_str_numeric(const char *str)
{
	if (!str || !*str)
		return (0);
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		str++;
	}
	return (1);
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
	if (current_shlvl_str && is_str_numeric(current_shlvl_str))
	{
		shlvl_num = ft_atoi(current_shlvl_str);
		if (shlvl_num >= 999)
			ft_putendl_fd("minishell: warning: shell level (1000) too high, resetting to 1", 2);
		if (shlvl_num >= 999)
			shlvl_num = 1;
		else
			shlvl_num++;
	}
	new_shlvl_str = ft_itoa(shlvl_num);
	export_arg = ft_strjoin("SHLVL=", new_shlvl_str);
	free(new_shlvl_str);
	export_args[0] = "export";
	export_args[1] = export_arg;
	export_args[2] = NULL;
	builtin_export(export_args, shell_context);
	free(export_arg);
}
