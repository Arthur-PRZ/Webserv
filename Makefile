CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv
SRCDIR = src
SRC = main.cpp Socket.cpp Server.cpp Location.cpp parser.cpp SendManagement.cpp RequestManagement.cpp

SRCS = $(addprefix $(SRCDIR)/,$(SRC))

all: $(NAME)

$(NAME): $(SRCS)
	@$(CC) $(CFLAGS) $(SRCS) -o $(NAME)
	@echo "Compiling with $(CFLAGS) flags"

clean:
	@rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
