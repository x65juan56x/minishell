NAME = konxi
SRC = 	konxitah.c \
		src/cmd_runner.c \
		src/find_the_path.c \

HEAD = konxitah.h
LIBFT_DIR = libs/libft
LIBFT = $(LIBFT_DIR)/libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBS = -lreadline -L$(LIBFT_DIR) -lft

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(SRC) $(HEAD) $(LIBFT)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME) $(LIBS)

clean:
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re