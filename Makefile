CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = 	srcs/main.cpp srcs/server/Server.cpp srcs/config/Config.cpp srcs/config/Parser.cpp \
		srcs/server/Client.cpp srcs/signals/Signals.cpp srcs/webserv/Webserv.cpp \
		srcs/webserv/epollUtils.cpp srcs/webserv/eventManager.cpp \
		srcs/request_part/request.cpp srcs/request_part/parseRequest.cpp \
		srcs/request_part/errorsParsing.cpp srcs/request_part/utils.cpp \
		srcs/response_part/response.cpp srcs/response_part/responseSetError.cpp \
		srcs/response_part/treatmentHeaders.cpp srcs/response_part/treatmentMethod.cpp \
		srcs/response_part/treatmentUri.cpp srcs/response_part/autoIndex.cpp \
		srcs/response_part/mimeTypes.cpp \
		srcs/cgi_part/Cgi.cpp srcs/cgi_part/parseCgi.cpp srcs/webserv/cgiEpoll.cpp

# OBJS = $(SRCS:.cpp=.o)
NAME = webserv

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
