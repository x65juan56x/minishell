NAME		= pipex
NAME_BONUS	= pipex_bonus
CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

INCLUDE_DIR	= include
SRC_DIR		= src

LIBFT_DIR	= libft

INCLUDES	= -I$(INCLUDE_DIR)

SRCS_FILES_NORMAL = pipex.c \
					cmd_runner.c \
					children_stuff.c \
					find_the_path.c

SRCS_FILES_BONUS_SPECIFIC = pipex_bonus.c \
							pipex_utils_bonus.c 

SHARED_UTIL_FILE = cmd_runner.c \
				   find_the_path.c

LIBFT_FILES	= ft_calloc.c \
			  ft_error.c \
			  ft_freearr.c \
			  ft_putstr_fd.c \
			  ft_split.c \
			  ft_strchr.c \
			  ft_strjoin.c \
			  ft_strlen.c \
			  ft_strncmp.c \
			  ft_substr.c \
			  ft_strdup.c \
			  ft_memset.c \
			  ft_bzero.c \
			  ft_split_args.c \
			  ft_mini_gnl.c

SRCS_NORMAL		= $(addprefix $(SRC_DIR)/, $(SRCS_FILES_NORMAL))
SRCS_BONUS_SPECIFIC = $(addprefix $(SRC_DIR)/, $(SRCS_FILES_BONUS_SPECIFIC))
SHARED_UTIL_SRC	= $(addprefix $(SRC_DIR)/, $(SHARED_UTIL_FILE))
LIBFT_SRCS		= $(addprefix $(LIBFT_DIR)/, $(LIBFT_FILES))

OBJS_NORMAL		= $(SRCS_NORMAL:.c=.o)
OBJS_BONUS_SPECIFIC = $(SRCS_BONUS_SPECIFIC:.c=.o)
SHARED_UTIL_OBJ	= $(SHARED_UTIL_SRC:.c=.o)
LIBFT_OBJS		= $(LIBFT_SRCS:.c=.o)

ALL_BONUS_OBJS = $(OBJS_BONUS_SPECIFIC) $(SHARED_UTIL_OBJ) $(LIBFT_OBJS)

ALL_SRC_OBJS = $(addprefix $(SRC_DIR)/, $(SRCS_FILES_NORMAL) $(SRCS_FILES_BONUS_SPECIFIC) $(SHARED_UTIL_FILE))
ALL_OBJS_FOR_CLEAN = $(ALL_SRC_OBJS:.c=.o) $(LIBFT_OBJS)


MAIN_HEADER = $(INCLUDE_DIR)/pipex.h

all: $(NAME)

$(NAME): $(OBJS_NORMAL) $(LIBFT_OBJS)
	$(CC) $(CFLAGS) $(OBJS_NORMAL) $(LIBFT_OBJS) -o $(NAME)

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(ALL_BONUS_OBJS)
	$(CC) $(CFLAGS) $(ALL_BONUS_OBJS) -o $(NAME_BONUS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(MAIN_HEADER)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT_DIR)/%.o: $(LIBFT_DIR)/%.c $(MAIN_HEADER)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(ALL_OBJS_FOR_CLEAN)

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re: fclean all

re_bonus: fclean bonus

.PHONY: all clean fclean re bonus re_bonus
