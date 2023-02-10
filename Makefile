ifeq ($(OS),Windows_NT)
NAME		=	corth.exe
else
NAME		=	corth
endif
CC			=	gcc
CPPFLAGS	=	-Wall -Wextra -Werror -g -Iinclude -Wno-unused-parameter
SRC			=	$(wildcard src/*.c)
OBJ			=	$(SRC:.c=.o)

.SILENT:

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

run:	$(NAME)
	./$(NAME)

clean:
ifeq ($(OS),Windows_NT)
	del /F /Q $(NAME)
else
	rm -rf $(NAME)
endif

fclean:	clean
ifeq ($(OS),Windows_NT)
	del /F /Q /S *.o
else
	rm -rf $(OBJ)
endif

re:	fclean	all

.PHONY:	all	clean	fclean	re