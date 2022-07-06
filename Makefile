CC=gcc
NAME=corth
SRC=corth.c
OBJ=*.o
CORTH_LIB=-Iinclude/ -Lsrc -lcorth
CFLAGS=-Wall -Wextra -Werror -Wdeprecated -Wno-comment -pedantic -g -O0
VALGRIND_LOGS_FILE=valgrind-out.txt
GDB_TMP_FILES=.gdb_history peda-session-*.txt
ASM_OUTPUT=output.asm
ASM_OBJ=$(ASM_OUTPUT:.asm=.o)
ASM=$(ASM_OUTPUT:.asm=)

all: $(NAME)

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$(VALGRIND_LOGS_FILE) ./$(NAME) sim examples/test.corth

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(CORTH_LIB)

$(OBJ): src/libcorth.a
	$(CC) $(CFLAGS) -c $(SRC)

src/libcorth.a:
	make -C src/

asm_it:
	nasm -felf64 -F dwarf output.asm -o output.o
	ld output.o -o output

clean:
	rm -f $(OBJ) $(GDB_TMP_FILES) $(ASM_OUTPUT)
	make -C src/ clean

fclean: clean
	rm -f $(NAME) $(ASM) $(VALGRIND_LOGS_FILE)
	make -C src/ fclean

re: fclean all

.PHONY: all clean fclean re
