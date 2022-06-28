CC=gcc
NAME=corth
SRC=*.c
OBJ=*.o
CORTH_LIB=-Iinclude/ -Lsrc -lcorth
CFLAGS=-Wall -Wextra -Werror -Wdeprecated -pedantic -g -O0
VALGRIND_LOGS_FILE=valgrind-out.txt
GDB_TMP_FILES=.gdb_history peda-session-*.txt
ASM_OUTPUT=output.asm
ASM_OBJ=$(ASM_OUTPUT:.asm=.o)
ASM=$(ASM_OUTPUT:.asm=)

all: $(NAME)

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$(VALGRIND_LOGS_FILE) ./$(NAME) com test.corth

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(CORTH_LIB)
	./$(NAME) com test.corth
	nasm -felf64 $(ASM_OUTPUT) -o $(ASM_OBJ)
	ld $(ASM_OBJ) -o $(ASM)
	make clean

$(OBJ): src/libcorth.a
	$(CC) $(CFLAGS) -c $(SRC) $(CORTH_LIB)

src/libcorth.a:
	make -C src/

clean:
	rm -f $(OBJ) $(GDB_TMP_FILES) $(ASM_OUTPUT)
	make -C src/ clean

fclean: clean
	rm -f $(NAME) $(ASM) $(VALGRIND_LOGS_FILE)
	make -C src/ fclean

re: fclean all

.PHONY: all clean fclean re
