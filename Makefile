
NAME_SERVER = webserv

CC = clang++

FLAGS = -Wall -Wextra -Werror

SRCS_SERVER = Server.cpp Request/Request.cpp Client/Client.cpp ConfigParser/ConfigParser.cpp utils.cpp

HEADERS = Server.hpp Request/Request.hpp Client/Client.hpp ConfigParser/ConfigParser.hpp utils.hpp

OBJS_SERVER = $(SRCS_SERVER:.cpp=.o)

all:$(NAME_SERVER)

$(NAME_SERVER): $(OBJS_SERVER) $(HEADERS)
	$(CC) main.cpp $(OBJS_SERVER) -o $(NAME_SERVER) 

clean:
	rm -rf $(OBJS_CLIENT) $(OBJS_SERVER)

fclean: clean
	rm -rf $(NAME_SERVER)

re: fclean all

.PHONY: all clean fclean re
