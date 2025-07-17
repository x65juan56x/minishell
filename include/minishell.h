#ifndef MINISHELL_H
# define  MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include "../libs/libft/libft.h"
# include <signal.h>
# include <termios.h>
# include <dirent.h>

#define CORAL_BOLD "\001\033[1;38;5;203m\002"
#define RESET_COLOR "\001\033[0m\002"
#define PROMPT CORAL_BOLD "MiniShell" RESET_COLOR " $ "

// La única variable global permitida, para el estado de la señal.
// 'volatile' evita que el compilador la optimice incorrectamente.
// 'sig_atomic_t' garantiza que su acceso sea una operación indivisible.
extern volatile sig_atomic_t	g_signal_status;

typedef enum e_token_type
{
	TOKEN_WORD,				// 0 - Palabras
	TOKEN_PIPE,				// 1 - |
	TOKEN_REDIRECT_IN,		// 2 - <
	TOKEN_REDIRECT_OUT,		// 3 - >
	TOKEN_REDIRECT_APPEND,	// 4 - >>
	TOKEN_HEREDOC,			// 5 - <<
	TOKEN_AND,				// 6 - && (bonus)
	TOKEN_OR,				// 7 - || (bonus)
	TOKEN_LPAREN,			// 8 - ( (bonus)
	TOKEN_RPAREN,			// 9 - ) (bonus)
	TOKEN_EOF				// 10 - Fin de tokens
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
	NODE_COMMAND,			// 0 - Comando simple
	NODE_PIPE,				// 1 - Pipe entre comandos
	NODE_REDIRECT_IN,		// 2 - Redirección entrada
	NODE_REDIRECT_OUT,		// 3 - Redirección salida
	NODE_REDIRECT_APPEND,	// 4 - Redirección append
	NODE_HEREDOC,			// 5 - Here document
	NODE_AND,				// 6 - AND lógico (bonus)
	NODE_OR					// 7 - OR lógico (bonus)
}	t_node_type;

typedef struct s_ast_node
{
	t_node_type			type;
	char				**args;		// Para comandos: ["echo", "hello", NULL]
	char				*file;		// Para redirects: "output.txt"
	struct s_ast_node	*left;		// Hijo izquierdo
	struct s_ast_node	*right;		// Hijo derecho
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
	int				exit_status;	// Estado de salida del shell
	t_list			*heredoc_files;	// Lista para rastrear ficheros heredoc
	char			**envp_cpy; // Copia del entorno del shell
} t_shell_context;


/* TOKENIZER */
t_token			*tokenize(const char *input);
void			cleanup_tokens(t_token *tokens);
t_token			*create_token(t_token_type type, char *value);
void			add_token(t_token **head, t_token **current, t_token *new_token);

/* TOKEN OPERATORS */
t_token_type	get_operator_type(const char *s, int *advance);
int				is_operator_char(char c);

/* TOKEN WORDS */
char			*extract_word_token(const char *s, int *i, int *quoted);
char			*process_quoted_string(const char *s, int start, int end, int *quoted);

/* PARSER AST */
t_ast_node		*parse(t_token *tokens);
void			cleanup_ast(t_ast_node *node);
t_ast_node		*create_ast_node(t_node_type type);
t_ast_node		*create_binary_node(t_token_type op_type, t_ast_node *left, t_ast_node *right);
t_ast_node		*create_redirect_node(t_token_type redirect_type, t_ast_node *cmd, char *file);

/* PARSER COMMANDS */
t_ast_node		*parse_command(t_parser *parser);
t_token			*consume_token(t_parser *parser, t_token_type expected);

/* PARSER EXPRESSIONS */
t_ast_node		*parse_or_expression(t_parser *parser);
t_ast_node		*parse_and_expression(t_parser *parser);
t_ast_node		*parse_pipe_expression(t_parser *parser);
t_ast_node		*parse_primary_expression(t_parser *parser);
t_ast_node		*parse_parenthesis_expression(t_parser *parser);
t_ast_node		*parse_redirect_expression(t_parser *parser);

/* PARSER REDIRECT UTILS */
int				is_redirect_token(t_token_type type);
int				is_redirect_node(t_node_type type);
int				is_logical_node(t_node_type type);

/* PARSER UTILS */
int				handle_word_token(t_token **tp, char **args, int *idx);
void			skip_redirect_token(t_token **tp);
int				extract_args(char **args, int max, t_token **tp);
t_token			*consume_token_type(t_parser *parser, t_token_type tp);
int				are_quotes_unclosed(const char *s);

/* WILDCARD EXPANSION */
t_token			*expand_wildcards(t_token *tokens);
int				match_wildcard(const char *str, const char *pattern);

/* EXPANDER */
void			is_expand_needed (char *s, int quoted, t_token *token);
void			expander_var(t_token *token_list, t_shell_context *shell_context);
char			*do_expand(t_token *token, int *i, t_shell_context *shell_context);

/* EXECUTOR */
int				execute_ast(t_ast_node *ast, int *heredoc_id_ptr, t_shell_context *shell_context);
int				execute_simple_command(t_ast_node *node, t_shell_context *shell_context);
void			launch_command(char **args, char **envp);

/* EXECUTOR UTILS */
void			print_signal_message(int signal_num);
int				analyze_child_status(int status);
int				apply_redirections(t_ast_node *node);

/* PIPE EXECUTOR */
int				execute_pipe_line(t_ast_node *ast, int *heredoc_id_ptr, t_shell_context *shell_context);
int				wait_for_all_children(pid_t *pids, int num_cmds);

/* PIPE EXECUTOR UTILS */
int				count_pipe_commands(t_ast_node *ast);
void			child_process_logic(t_pipe_state *st, int pipe_fd[2], int is_last, int *heredoc_id_ptr, t_shell_context *shell_context);
int				parent_process_logic(t_pipe_state *st, int pipe_fd[2]);
pid_t			create_pipe_child(t_ast_node *node, t_pipe_config *config, int *heredoc_id_ptr, t_shell_context *shell_context);

/* HEREDOC EXECUTOR */
int				execute_heredoc(const char *filename, char *delimiter);

/* HEREDOC UTILS */
void			disable_ctrl_echo(struct termios *orig_termios);
int				create_heredoc_file(const char *filename, char *delimiter);

/* HEREDOC PREPROCESSOR */
int				preprocess_heredocs(t_ast_node **node_ptr, int *heredoc_id_ptr, t_shell_context *shell_context);

/* PATH UTILS */
char			*find_command_path(char *cmd, char **envp);

/* SIGNALS */
void			setup_interactive_signals(void);
void			setup_child_signals(void);
void			setup_heredoc_signals(void);
void			ignore_signals(void);

/* BUILTINS */
int				is_builtin(char *cmd);
int				is_builtin_parent(char *cmd);
char			*get_env_value(const char *var_name, t_shell_context *shell_context);
int				execute_builtin(char **args, t_shell_context *shell_context);
int				builtin_echo(char **args);
int				builtin_pwd(void);
int				builtin_env(char **envp);
int				builtin_exit(char **args);
int				builtin_cd(char **args, t_shell_context *shell_context);
int				builtin_export(char **args, t_shell_context *shell_context);
int				builtin_unset(char **args, t_shell_context *shell_context);

/* SHELL MANAGEMENT */
char			**init_shell_environment(char **envp);
char			*get_user_input(void);
int				handle_input_line(char *input);
int				process_command_line(char *input, t_shell_context *shell_context);
int				run_shell_loop(t_shell_context *shell_context);
void			cleanup_heredoc_files(t_shell_context *shell_context);

#endif
