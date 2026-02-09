CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98
NAME    = webserv

# Source files
SRCS    = srcs/main.cpp srcs/server/Server.cpp srcs/server/Client.cpp

# Object files: Redirected to the obj/ directory
OBJ_DIR = obj
OBJS    = $(SRCS:srcs/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# Rule to compile .cpp files into .o files inside OBJ_DIR
$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

# CC = c++
# CFLAGS = -Wall -Wextra -Werror -std=c++98
# SRCS = srcs/main.cpp srcs/server/Server.cpp
# OBJS = $(SRCS:.cpp=.o)
# NAME = webserv

# all: $(NAME)

# $(NAME): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# %.o: %.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all

# .PHONY: all clean fclean re