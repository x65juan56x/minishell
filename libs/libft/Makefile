NAME = libft.a
SRC_DIR = .
OBJ_DIR = obj

SRC_FILES = ft_toupper.c \
			ft_isalpha.c \
			ft_isdigit.c \
			ft_tolower.c \
			ft_isalnum.c \
			ft_isascii.c \
			ft_strchr.c \
			ft_isprint.c \
			ft_strrchr.c \
			ft_strlen.c \
			ft_strcmp.c \
			ft_strncmp.c \
			ft_memset.c \
			ft_bzero.c \
			ft_memchr.c \
			ft_memcpy.c \
			ft_memcmp.c \
			ft_memmove.c \
			ft_strnstr.c \
			ft_strlcpy.c \
			ft_strlcat.c \
			ft_atoi.c \
			ft_calloc.c \
			ft_strdup.c \
			ft_substr.c \
			ft_strjoin.c \
			ft_strtrim.c \
			ft_split.c \
			ft_itoa.c \
			ft_strmapi.c \
			ft_striteri.c \
			ft_putchar_fd.c \
			ft_putstr_fd.c \
			ft_putendl_fd.c \
			ft_putnbr_fd.c \
			ft_error.c \
			ft_freearr.c \
			ft_split_args.c \
			ft_mini_gnl.c \
			ft_lstnew.c \
			ft_lstadd_front.c \
			ft_lstsize.c \
			ft_lstlast.c \
			ft_lstadd_back.c \
			ft_lstdelone.c \
			ft_lstclear.c \
			ft_lstiter.c \
			ft_lstmap.c

SRC = $(SRC_FILES)
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

HEAD = libft.h

CC = cc
CFLAGS = -Wall -Wextra -Werror

AR = ar rcs
RM = rm -f

GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
RESET = \033[0m

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(HEAD) | $(OBJ_DIR)
	@echo "$(GREEN)Compilando $<$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@echo "$(BLUE)Creando librería $(NAME)...$(RESET)"
	@$(AR) $(NAME) $(OBJ)
	@echo "$(GREEN)✓ $(NAME) creada exitosamente!$(RESET)"

clean:
	@echo "$(RED)Eliminando archivos objeto de libft...$(RESET)"
	@$(RM) -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Eliminando $(NAME)...$(RESET)"
	@$(RM) $(NAME)

re: fclean all

info:
	@echo "Archivos fuente: $(words $(SRC))"
	@echo "Archivos objeto: $(words $(OBJ))"
	@echo "Directorio objetos: $(OBJ_DIR)"

.PHONY: all clean fclean re info