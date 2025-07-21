NAME = get_next_line.a
SRC_DIR = .
OBJ_DIR = obj

SRC_FILES =	get_next_line.c get_next_line_utils.c
BONUS_SRC_FILES = get_next_line_bonus.c get_next_line_utils_bonus.c

HEAD = get_next_line.h get_next_line_bonus.h

CC = cc
CFLAGS = -Wall -Wextra -Werror

AR = ar rcs
RM = rm -f

GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
RESET = \033[0m

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))
BONUS_OBJ = $(addprefix $(OBJ_DIR)/, $(BONUS_SRC_FILES:.c=.o))

all: $(NAME)

bonus: OBJ_BONUS
	@echo "$(BLUE)Creando librería BONUS $(NAME)...$(RESET)"
	@$(AR) $(NAME) $(BONUS_OBJ)
	@echo "$(GREEN)✓ $(NAME) BONUS creada exitosamente!$(RESET)"

OBJ_BONUS: $(OBJ_DIR) $(BONUS_OBJ)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(HEAD) | $(OBJ_DIR)
	@echo "$(GREEN)Compilando $<$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJ)
	@echo "$(BLUE)Creando librería $(NAME)...$(RESET)"
	@$(AR) $(NAME) $(OBJ)
	@echo "$(GREEN)✓ $(NAME) creada exitosamente!$(RESET)"

clean:
	@echo "$(RED)Eliminando archivos objeto...$(RESET)"
	@$(RM) -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Eliminando $(NAME)...$(RESET)"
	@$(RM) $(NAME)

re: fclean all

info:
	@echo "Archivos fuente normal: $(words $(SRC_FILES))"
	@echo "Archivos fuente bonus: $(words $(BONUS_SRC_FILES))"
	@echo "Directorio objetos: $(OBJ_DIR)"

.PHONY: all bonus clean fclean re info
