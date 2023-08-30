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
	@echo $(GRAY) $(ITALIC) "Making .. $< " $(NONE)
	@g++-11 $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@g++-11 $(CPPFLAGS) $^ -o $(NAME)
	@echo $(GREEN) $(ITALIC) "Compiled ..." $(NONE)

clean:
	@rm -rf $(OBJ)
	@echo $(GREEN) $(ITALIC) "Objects removed ..." $(NONE)

fclean: clean
	@rm -rf $(NAME)
	@echo $(GREEN) $(ITALIC) "Cleaned ..." $(NONE)

re: fclean all

.PHONY: all clean fclean re
