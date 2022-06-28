CC=gcc
NAME=libcorth.a
SRC=*.c
OBJ=*.o
CFLAGS=-Wall -Wextra -Werror -Wdeprecated -pedantic -g -O0 -I../include/

all: $(NAME)

$(NAME): $(OBJ)
	ar rc $(NAME) $(OBJ)
	make clean

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
