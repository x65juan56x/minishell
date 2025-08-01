NAME = minishell
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC_FILES = minishell.c \
			executor/executor.c \
			executor/executor_utils.c \
			executor/executor_redirects.c \
			executor/execute_command.c \
			executor/pipe_executor.c \
			executor/pipe_executor_utils.c \
			executor/create_pipe_child.c \
			executor/wait_pipe_children.c \
			executor/heredoc_executor.c \
			executor/heredoc_utils.c \
			executor/heredoc_preprocessor.c \
			executor/command_launcher.c \
			executor/path_utils.c \
			parser/tokenizer.c \
			parser/token_utils.c \
			parser/token_operators.c \
			parser/token_words.c \
			parser/parser.c \
			parser/ast_utils.c \
			parser/parser_commands.c \
			parser/parser_expressions.c \
			parser/parser_utils.c \
			parser/parser_unclosed_utils.c \
			parser/parser_redirect_utils.c \
			parser/expander.c \
			parser/expander_utils.c \
			parser/expander_updater.c \
			parser/wildcard_expander.c \
			parser/wildcard_utils.c \
			signals/signals.c \
			signals/heredoc_signals.c \
			builtins/builtin_detector.c \
			builtins/builtin_implementations.c \
			builtins/builtin_cd.c \
			builtins/builtin_export.c \
			builtins/builtin_export_utils.c \
			builtins/builtin_unset.c \
			builtins/builtin_env_utils.c \
			shell/shell_init.c \
			shell/shell_input.c \
			shell/shell_level_init.c \
			shell/shell_process.c \
			shell/shell_loop.c \
			debugging/debugging.c \
			debugging/ast_debugging.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

HEAD = $(INCLUDE_DIR)/minishell.h
LIBFT_DIR = libs/libft
LIBFT = $(LIBFT_DIR)/libft.a
GNL_DIR = libs/get_next_line
GNL = $(GNL_DIR)/get_next_line.a

CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDE_DIR)
LIBS = -lreadline -L$(LIBFT_DIR) -lft -L$(GNL_DIR) -l:get_next_line.a

GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(LIBFT) $(GNL) $(NAME)

$(LIBFT):
	@echo "$(GREEN)Compiling libft...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR)

$(GNL):
	@echo "$(GREEN)Compiling get_next_line...$(RESET)"
	@$(MAKE) -C $(GNL_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/executor
	@mkdir -p $(OBJ_DIR)/parser
	@mkdir -p $(OBJ_DIR)/signals
	@mkdir -p $(OBJ_DIR)/builtins
	@mkdir -p $(OBJ_DIR)/shell
	@mkdir -p $(OBJ_DIR)/debugging

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEAD) | $(OBJ_DIR)
	@echo "$(GREEN)Compiling $<$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ) $(LIBFT) $(GNL)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)
	@echo "$(GREEN)✓ $(NAME) successfully compiled!$(RESET)"

clean:
	@echo "$(RED)Deleting object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean
	@$(MAKE) -C $(GNL_DIR) clean

fclean: clean
	@echo "$(RED)Deleting $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@$(MAKE) -C $(GNL_DIR) fclean

re: fclean all

info:
	@echo "Source files: $(words $(SRC))"
	@echo "Object files: $(words $(OBJ))"
	@echo "Object folder: $(OBJ_DIR)"

.PHONY: all clean fclean re info
