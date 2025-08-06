/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:23:17 by anagarri          #+#    #+#             */
/*   Updated: 2025/08/06 17:23:18 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include "../libs/libft/libft.h"
# include "../libs/get_next_line/get_next_line.h"
# include <signal.h>
# include <termios.h>
# include <dirent.h>

# define CORAL_BOLD "\001\033[1;38;5;203m\002"
# define RESET_COLOR "\001\033[0m\002"

// The only allowed global variable, used for signal state.
// 'volatile' prevents the compiler from optimizing it incorrectly.
// 'sig_atomic_t' ensures access is an indivisible operation.
extern volatile sig_atomic_t	g_signal_status;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_APPEND,
	TOKEN_HEREDOC,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_EOF
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
	int				expand;
	int				in_quotes;
}	t_token;

typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_PIPE,
	NODE_REDIRECT_IN,
	NODE_REDIRECT_OUT,
	NODE_REDIRECT_APPEND,
	NODE_HEREDOC,
	NODE_AND,
	NODE_OR
}	t_node_type;

typedef struct s_ast_node
{
	t_node_type			type;
	char				**args;
	char				*file;
	struct s_ast_node	*left;
	struct s_ast_node	*right;
}	t_ast_node;

typedef struct s_parser
{
	t_token		*tokens;
	t_token		*current;
	int			error;
}	t_parser;

typedef struct s_pipe_config
{
	int		*pipe_fd;
	int		is_left;
	int		heredoc_fd;
	char	***envp_ptr;
}	t_pipe_config;

typedef struct s_pipe_state
{
	pid_t		*pids;
	int			num_cmds;
	int			i;
	int			prev_pipe_fd;
	t_ast_node	*curr;
}	t_pipe_state;

typedef struct s_shell_context
{
	int				exit_status;
	int				should_exit;
	t_list			*heredoc_files;
	char			**envp_cpy;
	t_list			*local_vars;
	int				error_flag;
	/* Temporary references for child process cleanup */
	t_ast_node		*current_ast;
	t_token			*current_tokens;
}	t_shell_context;

typedef struct s_child_context
{
	int				*hd_id_ptr;
	t_shell_context	*shell_context;
}	t_child_context;

/* TOKENIZER */
t_token			*tokenize(const char *input);
void			cleanup_tokens(t_token *tokens);
t_token			*create_token(t_token_type type, char *value);
void			add_token(t_token **head, t_token **current,
					t_token *new_token);

/* TOKEN OPERATORS */
t_token_type	get_operator_type(const char *s, int *advance);
int				is_operator_char(char c);

/* TOKEN WORDS */
char			*extract_word_token(const char *s, int *i, int *quoted);
char			*process_quoted_string(const char *s, int start, int end,
					int *quoted);

/* PARSER AST */
t_ast_node		*parse(t_token *tokens, t_shell_context *shell_context);
void			cleanup_ast(t_ast_node *node);
t_ast_node		*create_ast_node(t_node_type type);
t_ast_node		*create_binary_node(t_token_type op_type, t_ast_node *left,
					t_ast_node *right);
t_ast_node		*create_redirect_node(t_token_type redirect_type,
					t_ast_node *cmd, char *file);

/* PARSER COMMANDS */
t_ast_node		*parse_command(t_parser *parser);
t_token			*consume_token(t_parser *parser, t_token_type expected);

/* PARSER EXPRESSIONS */
t_ast_node		*parse_logical_expression(t_parser *parser);
t_ast_node		*parse_pipe_expression(t_parser *parser);
t_ast_node		*parse_primary_expression(t_parser *parser);
t_ast_node		*parse_parenthesis_expression(t_parser *parser);
t_ast_node		*parse_redirect_expression(t_parser *parser);

/* PARSER EXPRESSIONS UTILS */

char			**convert_arg_list_to_array(t_list *arg_list);
int				process_tokens_loop(t_parser *parser, t_list **arg_list,
					t_ast_node **node);

/* PARSER REDIRECT UTILS */
int				is_redirect_token(t_token_type type);
int				is_redirect_node(t_node_type type);

/* PARSER UTILS */
int				handle_word_token(t_token **tp, char **args, int *idx);
void			skip_redirect_token(t_token **tp);
int				extract_args(char **args, int max, t_token **tp);
t_token			*consume_token_type(t_parser *parser, t_token_type tp);
int				are_quotes_unclosed(const char *s);
int				are_parentheses_unclosed(const char *s);
int				has_pending_logical_operator(const char *input);

/* WILDCARD EXPANSION */
t_token			*expand_wildcards(t_token *tokens);
int				match_wildcard(const char *str, const char *pattern);

/* EXPANDER */
void			is_expand_needed(char *s, int quoted, t_token *token);
void			expander_var(t_token *token_list,
					t_shell_context *shell_context);
char			*expander_line_content(char *line,
					t_shell_context *shell_context);
void			update_tokens_values(t_shell_context *shell_context,
					t_token *tmp);

/* EXPANDER UTILS*/
char			*expand_pid(int *i);
char			*expand_status(int *i, t_shell_context *shell_context);
char			*extract_var_name(char *str, int *i);
char			*expand_var(int *i, t_shell_context *shell_context,
					t_token *token);
char			*expand_curly(int *i, t_shell_context *shell_context,
					t_token *token);

/* EXECUTOR */
int				execute_ast(t_ast_node *ast, int *heredoc_id_ptr,
					t_shell_context *shell_context);
int				execute_simple_command(t_ast_node *node,
					t_shell_context *shell_context);
void			launch_command(char **args, char **envp, t_shell_context *shell_context);
int				handle_variable_assignment(char **args,
					t_shell_context *context);
void			print_signal_message(int signal_num);
int				analyze_child_status(int status);
int				apply_redirections(t_ast_node *node);

/* PIPE EXECUTOR */
int				execute_pipe_line(t_ast_node *ast, int *heredoc_id_ptr,
					t_shell_context *shell_context);
int				wait_for_all_children(pid_t *pids, int num_cmds);
int				count_pipe_commands(t_ast_node *ast);
void			child_process_logic(t_pipe_state *st, int pipe_fd[2],
					int is_last, t_child_context *ctx);
int				parent_process_logic(t_pipe_state *st, int pipe_fd[2]);
pid_t			create_pipe_child(t_ast_node *node, t_pipe_config *config,
					int *heredoc_id_ptr, t_shell_context *shell_context);

/* HEREDOC */
int				execute_heredoc(const char *filename, char *delimiter,
					t_shell_context *shell_context);
int				create_heredoc_file(const char *filename, char *delimiter,
					t_shell_context *shell_context);
int				preprocess_heredocs(t_ast_node **node_ptr, int *heredoc_id_ptr,
					t_shell_context *shell_context);

/* PATH UTILS */
char			*find_command_path(char *cmd, char **envp);

/* SIGNALS */
void			setup_interactive_signals(void);
void			setup_child_signals(void);
void			setup_heredoc_signals(void);
void			ignore_signals(void);

/* BUILTINS */
int				execute_builtin(char **args, t_shell_context *shell_context);
int				builtin_echo(char **args);
int				builtin_pwd(void);
int				builtin_env(char **envp, char **args);
int				builtin_exit(char **args, t_shell_context *shell_context);
int				builtin_cd(char **args, t_shell_context *shell_context);
int				builtin_export(char **args, t_shell_context *shell_context);
int				builtin_unset(char **args, t_shell_context *shell_context);
int				get_exit_status_from_args(t_token *args_token);
int				find_local_var(const char *name, t_list *local_vars);
void			remove_local_var(const char *name, t_list **local_vars);

/* BUILTINS UTILS */
int				is_builtin(char *cmd);
void			update_shell_level(t_shell_context *shell_context);
int				is_builtin_parent(char *cmd);
char			*get_env_value(const char *var_name,
					t_shell_context *shell_context);
int				is_valid_identifier(const char *name);
int				add_new_env_var(const char *arg,
					t_shell_context *shell_context);
int				find_env_var_index(const char *name,
					t_shell_context *shell_context);
void			remove_local_var(const char *name, t_list **local_vars);
void			sort_and_print_export(char **envp_cpy);

/* SHELL MANAGEMENT */
char			**init_shell_environment(char **envp,
					t_shell_context *shell_context);
char			*get_user_input(void);
int				handle_input_line(char *input);
int				handle_multiline_input(char **input_ptr);
int				process_command_line(t_token *tokens,
					t_shell_context *shell_context);
int				run_shell_loop(t_shell_context *shell_context);
void			cleanup_heredoc_files(t_shell_context *shell_context);
void			cleanup_shell_context(t_shell_context *shell_context);
void			cleanup_child_process(t_shell_context *shell_context);
void			cleanup_child_process_basic(t_shell_context *shell_context);
void			cleanup_child_process_deep(t_shell_context *shell_context);
void			cleanup_child_process_with_parsing(t_shell_context *shell_context,
					t_ast_node *ast, t_token *tokens);
void			force_global_cleanup(void);

/* SHELL MULTILINE UTILS */
void			cancel_multiline_input(char **input_ptr);
int				needs_multiline(char *input);
int				should_handle_logical_ops(char *input);

/* DEBUGGING */
void			debug_print_local_vars(t_shell_context *ctx);
void			debug_print_token_list(t_token *head);
void			debug_print_expand(t_token token);
void			debug_print_ast(const t_ast_node *node);
void			debug_print_parser(const t_parser *parser);

#endif
