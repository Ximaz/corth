CC=gcc
NAME=corth
SRC=*.c src/*.c
OBJ=*.o
CFLAGS=-Wall -Wextra -Werror -Wdeprecated -pedantic -Iinclude/
VALGRIND_LOGS_FILE=valgrind-out.txt
GDB_TMP_FILES=.gdb_history peda-session-*.txt
ASM_OUTPUT=output.asm
ASM_OBJ=$(ASM_OUTPUT:.asm=.o)
ASM=$(ASM_OUTPUT:.asm=)

all: $(NAME)

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$(VALGRIND_LOGS_FILE) ./$(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	./$(NAME) com $(ASM_OUTPUT)
	nasm -felf64 $(ASM_OUTPUT) -o $(ASM_OBJ)
	ld $(ASM_OBJ) -o $(ASM)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ) $(VALGRIND_LOGS_FILE) $(GDB_TMP_FILES) $(ASM_OUTPUT)

fclean: clean
	rm -f $(NAME) $(ASM)

re: fclean all

.PHONY: all clean fclean re
