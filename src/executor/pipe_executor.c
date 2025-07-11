#include "../../include/minishell.h"

static void	child_process_routine(t_ast_node *node, t_pipe_config *config)
{
    int	exit_code;

    setup_child_signals();
    if (config->is_left)
    {
        if (config->heredoc_fd != -1)
            dup2(config->heredoc_fd, STDIN_FILENO);
        dup2(config->pipe_fd[1], STDOUT_FILENO);
    }
    else
    {
        dup2(config->pipe_fd[0], STDIN_FILENO);
    }
    close(config->pipe_fd[0]);
    close(config->pipe_fd[1]);
    if (config->heredoc_fd != -1)
        close(config->heredoc_fd);
    exit_code = execute_ast(node, config->envp_ptr);
    exit(exit_code);
}

pid_t	create_pipe_child(t_ast_node *node, t_pipe_config *config)
{
    pid_t	pid;

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (-1);
    }
    if (pid == 0)
    {
        child_process_routine(node, config);
    }
    return (pid);
}

int	wait_pipe_children(pid_t left_pid, pid_t right_pid)
{
    int	left_status;
    int	right_status;

    if (left_pid > 0)
        waitpid(left_pid, &left_status, 0);
    if (right_pid > 0)
        waitpid(right_pid, &right_status, 0);
    if (right_pid > 0 && WIFEXITED(right_status))
        return (WEXITSTATUS(right_status));
    if (right_pid > 0 && WIFSIGNALED(right_status))
        return (128 + WTERMSIG(right_status));
    if (left_pid > 0 && WIFEXITED(left_status))
        return (WEXITSTATUS(left_status));
    if (left_pid > 0 && WIFSIGNALED(left_status))
        return (128 + WTERMSIG(left_status));
    return (1);
}
