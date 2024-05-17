NAME := exe
CXX := c++

CXXFLAGS := -std=c++11 -Wall -Wextra -g -fsanitize=address
# CXXFLAGS := -Werror
RM := rm -rf

SRC := $(shell find src -type f -name '*.cpp')

INC:=	$(wildcard inc/*.hpp)

OBJ_DIR :=	./obj
OBJ := $(addprefix $(OBJ_DIR)/,$(SRC:src/%.cpp=%.o))

OBJ_DIRS := $(sort $(dir $(OBJ)))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -Iinc $^ -o $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp $(INC) | $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) -Iinc -c $< -o $@

$(OBJ_DIRS):
	mkdir -p $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME)

.PHONY: clean all fclean re run

