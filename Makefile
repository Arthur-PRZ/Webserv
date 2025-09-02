CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv
SRCDIR = src
SRC = main.cpp

all: $(NAME)

$(NAME): $(SRCDIR)/$(SRC)
	@$(CC) $(CFLAGS) $(SRCDIR)/$(SRC) -o $(NAME)
	@echo "Compiling with $(CFLAGS) flags"

clean:
	@rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all debug clean fclean re