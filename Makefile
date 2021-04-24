
.PHONY: all re fclean clean

NAME = webserv

CGI_SRC = CGI/CGI.cpp CGI/RequestConfigMatch.cpp

CLIENT_SRC = Client/Client.cpp

SERVER_SRC = Server/Server.cpp

CONFIG_PARSER_SRC = ConfigParser/ConfigParser.cpp

REQUEST_SRC = Request/Request.cpp

RESPONSE_SRC = Response/Response.cpp

UTILS_SRC = utils/utils.cpp utils/MIMEtypes.cpp utils/errors.cpp

DIR_SRC = srcs/

DIR_OBJ = objects/

SRCS = $(addprefix $(DIR_SRC),$(CGI_SRC) $(CLIENT_SRC) $(SERVER_SRC) $(CONFIG_PARSER_SRC) $(REQUEST_SRC) $(RESPONSE_SRC) $(UTILS_SRC) startServer.cpp)

OBJS = $(addprefix $(DIR_OBJ),$(SRCS:.c=.o))

SRCO = $(SRCS:.c=.o)

CC = clang++

OK_COLOR=\x1b[32;01m

DELETE_COLOR=\x1b[31;01m

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -g startServer.cpp $(OBJS) -o $(NAME)

$(DIR_OBJ)%.o: $(DIR_SRC)%.c $(DIR_SRC)parser.h
	@mkdir -p objects
	@$(CC) -I $(DIR_SRC) -c $< -o $@

clean:
	rm -rf $(OBJS_CLIENT) $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
