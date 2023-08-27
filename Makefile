NAME = webserv
INC = -I include
SANITIZE = -fsanitize=address -g3

NONE = '\033[0m'
GREEN = '\033[0;32m'
GRAY = '\033[2;37m'
ITALIC = '\033[3m'

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 $(INC) $(SANITIZE)

SRC = src/webserv.cpp\
	src/Server.cpp\
	src/Cluster.cpp\
	src/Client.cpp\
	src/Request.cpp\
	src/Response.cpp\
	src/parsing.cpp\
	src/exec_cgi.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	@echo -e $(GRAY) $(ITALIC) "Making .. $< " $(NONE)
	@c++ $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@c++ $(CPPFLAGS) $^ -o $(NAME)
	@echo -e $(GREEN) $(ITALIC) "Compiled ..." $(NONE)

clean:
	@rm -rf $(OBJ)
	@echo -e $(GREEN) $(ITALIC) "Objects removed ..." $(NONE)

fclean: clean
	@rm -rf $(NAME)
	@echo -e $(GREEN) $(ITALIC) "Cleaned ..." $(NONE)

re: fclean all

.PHONY: all clean fclean re
