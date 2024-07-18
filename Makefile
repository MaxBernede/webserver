NAME := exe
CXX := c++

CXXFLAGS = -std=c++20 -Wall -Wextra -g 
# CXXFLAGS += -fsanitize=address
# CXXFLAGS += -Werror
RM := rm -rf

SRC_DIR := src
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp') 

INC := $(wildcard inc/*.hpp)

RUN_CMD := ./$(NAME) template.conf

OBJ_DIR := obj
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
OBJ_DIRS := $(sort $(dir $(OBJS))) 

# mkdir saved_files
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -Iinc $^ -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) -Iinc -c $< -o $@

clean:
	$(RM) saved_files/*
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

run: all
	$(RUN_CMD)

.PHONY: clean all fclean re run
	
