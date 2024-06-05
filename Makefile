NAME := exe
CXX := c++

CXXFLAGS := -std=c++11 -Wall -Wextra -g -fsanitize=address
# CXXFLAGS := -Werror
RM := rm -rf

SRC := $(shell find src -type f -name '*.cpp')

INC := $(wildcard inc/*.hpp)

RUN_CMD := ./$(NAME) template.conf

OBJ_DIR := ./obj
OBJ := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

OBJ_DIRS := $(sort $(dir $(OBJ)))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -Iinc $^ -o $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) -Iinc -c $< -o $@

$(OBJ_DIRS):
	mkdir -p $@

clean:
	$(RM) saved_files/*
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

run: all
	$(RUN_CMD)	

.PHONY: clean all fclean re run
