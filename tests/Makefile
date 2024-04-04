NAME := exe
CXX := c++

CXXFLAGS := -std=c++11 -fsanitize=address -g #-Wall -Wextra -Werror
RM := rm -rf

SRC := $(wildcard *.cpp)

INC:=	$(wildcard *.hpp)

OBJ_DIR :=	./obj
OBJ		:=	$(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp $(INC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

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

