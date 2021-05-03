
.PHONY: all clean fclean re

NAME		=	webserv

SRCS		=	RequestConfigMatch.cpp Client.cpp Server.cpp ConfigParser.cpp Request.cpp Response.cpp \
				CGI.cpp MIMEtypes.cpp errors.cpp signals.cpp startServer.cpp utils.cpp 

CONFIG		= ./webserv.conf

SRCS_DIR		= ./srcs/

HEADERS_DIR		= ./includes/

OBJECTS_DIR		=	./objects

OBJECTS			=	$(addprefix $(OBJECTS_DIR)/, $(SRCS:cpp=o))

DEPS			=	$(OBJECTS:%.o=%.d)

CC				=	clang++

FLAGS			=	-std=c++98 -g


DELETE_COLOR	= \x1b[31;01m
OK_COLOR		= \x1b[32;01m
COMPINIG_COLOR	= \033[35;5;3m
INFORM_COLOR 	= \033[36;5;1m

all: $(NAME)

$(OBJECTS_DIR)/%.o: $(SRCS_DIR)%.cpp | $(OBJECTS_DIR)
	@echo "\n$(COMPINIG_COLOR)Compiling \n$<\033[0m"
	$(CC) $(FLAGS) -MMD -I $(HEADERS_DIR) -c $< -o $@

$(NAME): $(OBJECTS)
	@echo "\n$(COMPINIG_COLOR)Compiling $@\n\033[0m"
	$(CC) $(FLAGS) $^ -o $@
	@echo "\n$(OK_COLOR)Webserver compiled successfully\n\033[0m"
	@echo "\n$(INFORM_COLOR)Please, run make test or make start\nElse start Webserver with configuration file\033[0m\n\n"

-include $(DEPS)

$(OBJECTS_DIR):
	@mkdir -p $(OBJECTS_DIR)

clean:
	@rm -rf $(OBJECTS_DIR)
	@echo "\n$(DELETE_COLOR)Webserver's object files deleted\033[0m\n"

fclean: clean
	@rm -f $(NAME)
	@echo "$(DELETE_COLOR)Binary file deleted\033[0m\n"

start: all
	./$(NAME) $(CONFIG)

test: all
	./$(NAME) $(CONFIG) & ./tester http://localhost:8081

re: fclean all
