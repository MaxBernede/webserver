NAME := exe
CXX := c++

CXXFLAGS := -std=c++11 -g -fsanitize=address #-Wall -Wextra -Werror
RM := rm -rf

# Use find to get all .cpp files in src and its subdirectories
SRC := $(shell find src -type f -name '*.cpp')

# Include headers
INC := $(wildcard inc/*.hpp)

# Object files directory
OBJ_DIR := ./obj

# Convert .cpp file paths to corresponding .o file paths in OBJ_DIR
OBJ := $(patsubst src/%, $(OBJ_DIR)/%, $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -Iinc $^ -o $(NAME)

# Rule to create object files from source files
$(OBJ_DIR)/%.o: src/%.cpp $(INC) | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iinc -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) saved_files/*
	$(RM) $(OBJ)

fclean: clean
	$(RM) saved_files/*
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME)

.PHONY: clean all fclean re run
