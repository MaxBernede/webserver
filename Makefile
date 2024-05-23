NAME := exe
CXX := c++

CXXFLAGS := -std=c++11 -Wall -Wextra -Werror -g -fsanitize=address
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
	$(RM) saved_files/*
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME)



CONFIG_FILE = template.conf
TEST_SCRIPT = ./testScript.sh

test: all run_server test_webserver

run_server:
	@echo "Running web server..."
	@./$(NAME) $(CONFIG_FILE) > /dev/null 2>&1 &
	@sleep 1

test_webserver:
	@echo "Running web server tests..."
	@$(TEST_SCRIPT)

.PHONY: clean all fclean re run test run_server test_webserver

