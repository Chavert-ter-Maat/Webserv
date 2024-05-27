# Directories and File Names
NAME        := Webserv
SRC_DIR     := src
BUILD_DIR   := obj
MAIN        := main.cpp
RM          := rm -rf
HEADERS     := inc/Webserv.hpp
CC          := c++ --std=c++11

# Include Paths
INCLUDES    := -I ./inc

# Compiler Flags
CFLAGS     	:= 
# CFLAGS	:= -Wall -Wextra -Werror -Wunreachable-code -Ofast

# Source files
SRC         := $(wildcard $(SRC_DIR)/**/*.cpp)

# Object files
OBJS        := $(addprefix $(BUILD_DIR)/, $(SRC:$(SRC_DIR)/%.cpp=%.o))
MAIN_OBJ    := $(addprefix $(BUILD_DIR)/, $(MAIN:%.cpp=%.o))

# Colors
RED		  	:= \033[31m
BLUE	  	:= \033[34m
YELLOW		:= \033[33m
GREEN    	:= \033[32m
RESET_COLOR := \033[0m

# Targets
all: $(NAME)

$(NAME): $(OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(NAME)
	@echo "$(GREEN)$(NAME) executable created$(RESET_COLOR)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Cleaning Targets
clean:
	find ./obj -name "*.o" -type f -delete
	@echo "$(YELLOW)Object files deleted$(RESET_COLOR)"
	
fclean: clean
	$(RM) $(NAME)
	@echo "$(RED)$(NAME) executable deleted$(RESET_COLOR)"

re: fclean all

.PHONY: all clean fclean re