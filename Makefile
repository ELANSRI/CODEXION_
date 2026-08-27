NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread

SRC         = codexion.c \
              parsing.c \
              init.c \
              threads.c \
              routine.c \
              edf_utils.c \
              scheduler.c \
              monitor.c \
              utils.c

OBJ         = $(SRC:.c=.o)

HEADER      = codexion.h

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

