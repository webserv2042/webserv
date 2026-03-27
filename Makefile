# Colors and Style
PINK    = \033[1;35m
BLUE    = \033[1;34m
GREEN   = \033[1;32m
YELLOW  = \033[1;33m
RESET   = \033[0m
CLEAR   = \r\033[K

# Compiler settings
CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98

# Project files
NAME    = webserv
OBJ_DIR = obj
SRCS    = srcs/main.cpp srcs/server/Server.cpp srcs/config/Config.cpp srcs/config/Parser.cpp \
          srcs/server/Client.cpp srcs/signals/Signals.cpp srcs/webserv/Webserv.cpp \
          srcs/webserv/epollUtils.cpp srcs/webserv/eventManager.cpp \
          srcs/request_part/request.cpp srcs/request_part/parseRequest.cpp \
          srcs/request_part/errorsParsing.cpp srcs/request_part/utils.cpp \
          srcs/response_part/response.cpp srcs/response_part/responseSetError.cpp \
          srcs/response_part/treatmentHeaders.cpp srcs/response_part/treatmentMethod.cpp \
          srcs/response_part/treatmentUri.cpp srcs/response_part/autoIndex.cpp \
          srcs/response_part/mimeTypes.cpp \
          srcs/cgi_part/Cgi.cpp srcs/cgi_part/parseCgi.cpp

OBJS    = $(SRCS:srcs/%.cpp=$(OBJ_DIR)/%.o)

# Progress variables
TOTAL_FILES = $(words $(SRCS))
CURRENT_FILE = 0

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(CLEAR)$(YELLOW)🚀 Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✅ Compilation finished! [$(NAME) is ready]$(RESET)"

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@$(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
	@printf "$(CLEAR)$(BLUE)🛠️  Compiling [%d/%d]:$(RESET) %s" $(CURRENT_FILE) $(TOTAL_FILES) $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(PINK)🧹 Cleaning up objects...$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(PINK)🗑️  Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re