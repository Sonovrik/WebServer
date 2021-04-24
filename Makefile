
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

ALL_SRCS = RequestConfigMatch.cpp Client.cpp Server.cpp ConfigParser.cpp Request.cpp Response.cpp \
				utils.cpp MIMEtypes.cpp errors.cpp startServer.cpp

SRCS = $(addprefix $(DIR_SRC), $(CGI_SRC) $(CLIENT_SRC) $(SERVER_SRC) $(CONFIG_PARSER_SRC) $(REQUEST_SRC) $(RESPONSE_SRC) $(UTILS_SRC) startServer.cpp)

SRCSH = $(CGI_SRC:.cpp=.hpp) $(CLIENT_SRC:.cpp=.hpp) $(SERVER_SRC:.cpp=.hpp) $(CONFIG_PARSER_SRC:.cpp=.hpp) $(REQUEST_SRC:.cpp=.hpp) $(RESPONSE_SRC:.cpp=.hpp) utils/utils.hpp

CC = clang++

OBJS = $(addprefix $(DIR_OBJ),$(ALL_SRCS:.cpp=.o))

SRCO = $(SRCS:.cpp=.o)

OK_COLOR=\x1b[32;01m

DELETE_COLOR=\x1b[31;01m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(SRCS) -o $(NAME)
	@echo "$(OK_COLOR)Done"
	@tput sgr0

$(DIR_OBJ)%.o: $(SRCS) $(SRCSH)
	@mkdir -p objects
	@$(CC) -I $(DIR_SRC) -c $< -o $@

# clean:
# 	rm -rf $(OBJS_CLIENT) $(OBJS)

# fclean: clean
# 	rm -rf $(NAME)

# re: fclean all
