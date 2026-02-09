CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = srcs/main.cpp srcs/server/Server.cpp srcs/config/Config.cpp srcs/config/Parser.cpp
OBJS = $(SRCS:.cpp=.o)
NAME = webserv

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
