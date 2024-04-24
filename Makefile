NAME := exe
CXX := c++

CXXFLAGS := -std=c++11 -g -fsanitize=address -Wall -Wextra -Werror
RM := rm -rf

SRC := $(wildcard src/*.cpp)

INC:=	$(wildcard inc/*.hpp)

OBJ_DIR :=	./obj
OBJ		:= $(addprefix $(OBJ_DIR)/,$(notdir $(SRC:.cpp=.o)))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -Iinc $^ -o $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp $(INC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -Iinc -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME)

.PHONY: clean all fclean re run

