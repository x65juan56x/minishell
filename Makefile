NAME = minishell
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC_FILES = minishell.c \
			executor/path_utils.c \
			executor/executor.c \
			executor/pipe_executor.c \
			executor/redirect_executor.c \
			executor/heredoc_executor.c \
			executor/heredoc_preprocessor.c \
			executor/cmd_executor.c \
			parser/tokenizer.c \
			parser/token_utils.c \
			parser/token_operators.c \
			parser/token_words.c \
			parser/parser.c \
			parser/ast_utils.c \
			parser/parser_commands.c \
			parser/parser_expressions.c \
			parser/parser_utils.c \
			parser/parser_redirect_utils.c \
			signals/signals.c \
			builtins/builtin_detector.c \
			builtins/builtin_implementations.c \
			builtins/builtin_cd.c \
			builtins/builtin_env_utils.c \
			shell/shell_init.c \
			shell/shell_input.c \
			shell/shell_process.c \
			shell/shell_loop.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

HEAD = $(INCLUDE_DIR)/minishell.h
LIBFT_DIR = libs/libft
LIBFT = $(LIBFT_DIR)/libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDE_DIR)
LIBS = -lreadline -L$(LIBFT_DIR) -lft

GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(LIBFT) $(NAME)

$(LIBFT):
	@echo "$(GREEN)Compilando libft...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/executor
	@mkdir -p $(OBJ_DIR)/parser
	@mkdir -p $(OBJ_DIR)/signals
	@mkdir -p $(OBJ_DIR)/builtins
	@mkdir -p $(OBJ_DIR)/shell

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEAD) | $(OBJ_DIR)
	@echo "$(GREEN)Compilando $<$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ) $(LIBFT)
	@echo "$(GREEN)Linkeando $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)
	@echo "$(GREEN)✓ $(NAME) compilado exitosamente!$(RESET)"

clean:
	@echo "$(RED)Eliminando archivos objeto...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(RED)Eliminando $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

info:
	@echo "Archivos fuente: $(words $(SRC))"
	@echo "Archivos objeto: $(words $(OBJ))"
	@echo "Directorio objetos: $(OBJ_DIR)"

.PHONY: all clean fclean re info