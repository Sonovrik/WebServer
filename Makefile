
NAME_SERVER = server
CC = clang++
FLAGS = -Wall -Wextra -Werror
SRCS_SERVER = server.cpp

OBJS_SERVER = $(SRCS_SERVER:.cpp=.o)

all:$(NAME_SERVER)

$(NAME_SERVER): $(OBJS_SERVER)
	$(CC) -o $(NAME_SERVER) $(OBJS_SERVER)

clean:
	rm -rf $(OBJS_CLIENT) $(OBJS_SERVER)

fclean: clean
	rm -rf $(NAME_SERVER)

re: fclean all

.PHONY: all clean fclean re
