NAME = game_of_life

OS := $(shell uname)
ifeq ($(OS),Darwin)
  CXX=clang++
  INCLUDE_AND_LIBS = -I include
  FLAGS = -std=c++14 -Wall -Wextra -Werror -O2 -Wno-unused-command-line-argument \
			-Wno-unused -Wno-unused-parameter
else
  CXX=g++
  INCLUDE_AND_LIBS = -I include
  FLAGS = -std=c++14 -Wall -Wextra -Werror -O2 -Wno-unused-command-line-argument \
			-Wno-unused -Wno-unused-parameter
endif


OBJDIR = obj/

SRCDIR = src/

_SRC = 	main.cpp \
		GameOfLife.cpp \

SRC = $(addprefix $(SRCDIR), $(_SRC))

OBJ = $(addprefix $(OBJDIR),$(_SRC:.cpp=.o))

all: make_dir $(NAME)

make_dir:
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(FLAGS) $(INCLUDE_AND_LIBS) -o $@ -c $<

$(NAME): $(OBJ) $(SRC)
	$(CXX) $(FLAGS) $(INCLUDE_AND_LIBS) -o $(NAME) $(OBJ)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all