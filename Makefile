# DEFINE COLOR AND STYLE CODES
RESET = \033[0m
BOLD = \033[1m
ITALIC = \033[3m
UNDERLINE = \033[4m
RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
MAGENTA = \033[35m
CYAN = \033[36m
LIGHTPINK = \033[38;5;217m
GRAY = \033[90m

# SILENCE THE ENTERING/LEAVING DIRECTORY MESSAGE
#MAKEFLAGS += --no-print-directory

# COMPILER AND FLAGS
CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

# SOURCE 
SRC = main.cpp

# DIRECTORIES
OBJ_DIR = obj
SRC_DIR = src

# DEFINE OBJECTS
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

# PROGRAM NAME
NAME = webserv

# DEFAULT TARGET
all: $(NAME)
	@echo "${BOLD}webserv ready 🌐${RESET}"

# LINKING EXECUTABLE
$(NAME): $(OBJ_DIR) $(OBJ)
	@${CC} ${CPPFLAGS} $(OBJ) -o $(NAME)

# CREATING OBJECT DIRECTORIES
$(OBJ_DIR):
	@mkdir -p $@

# COMPILING .c FILES INTO .o FILES (INSIDE OBJ DIR)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "${ITALIC}Compiling ✰ $< ${RESET}"
	@${CC} ${CPPFLAGS} -c $< -o $@

# BASIC RULES
clean:
	@rm -rf $(OBJ_DIR)
	@echo "${ITALIC}Cleaning object files${RESET}"

fclean: clean
	@rm -rf $(NAME)
	@echo "${ITALIC}Cleaning everything${RESET}"

re: fclean all

.PHONY: all clean fclean re