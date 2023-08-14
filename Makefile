NAME = webserv
INC = -I include -I src/parse
SANITIZE = -fsanitize=address -g3

NONE = '\033[0m'
GREEN = '\033[0;32m'
GRAY = '\033[2;37m'
ITALIC = '\033[3m'

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 $(INC) $(SANITIZE)

SRC = src/multiplex/webserv.cpp\
	src/multiplex/Server.cpp\
	src/multiplex/Cluster.cpp\
	src/multiplex/Client.cpp\
	src/multiplex/Request.cpp\
	src/multiplex/Response.cpp\
	src/parse/parsing.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	@echo  $(GRAY) $(ITALIC) "Making .. $< " $(NONE)
	@c++ $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@c++ $(CPPFLAGS) $^ -o $(NAME)
	@echo  $(GREEN) $(ITALIC) "Compiled .." $(NONE)

clean:
	@rm -rf $(OBJ)
	@echo  $(GREEN) $(ITALIC) "Objects removed .." $(NONE)

fclean: clean
	@rm -rf $(NAME)
	@echo  $(GREEN) $(ITALIC) "Cleaned .. " $(NONE)

re: fclean all

.PHONY: all clean fclean re
