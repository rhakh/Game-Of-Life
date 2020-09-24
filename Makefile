NAME = game_of_life

OS := $(shell uname)
ifeq ($(OS),Darwin)
  CXX=clang++
  INCLUDE_AND_LIBS = -I include \
					-I /usr/local/Cellar/boost/1.72.0_3/include \
					-L /usr/local/Cellar/boost/1.72.0_3/lib
  FLAGS = -std=c++14 -Wall -Wextra -Werror -O2 \
			-Wno-unused -Wno-unused-parameter -Wno-unused-result -Wno-unused-command-line-argument \
			-lboost_filesystem  -lboost_system  -lboost_program_options \
			-pthread -lboost_thread-mt
else
  CXX=g++
  INCLUDE_AND_LIBS = -I include \
					-L /usr/lib/x86_64-linux-gnu \
					-I /usr/include/boost
  FLAGS = -std=c++14 -Wall -Wextra -Werror -O2 \
			-Wno-unused -Wno-unused-parameter -Wno-unused-result -Wno-unused-command-line-argument \
			-lboost_filesystem  -lboost_system -lboost_program_options \
			-pthread -lboost_thread
endif


OBJDIR = obj/

SRCDIR = src/

_SRC = 	main.cpp \
		GameOfLife.cpp \
		CLI.cpp \

SRC = $(addprefix $(SRCDIR), $(_SRC))

OBJ = $(addprefix $(OBJDIR),$(_SRC:.cpp=.o))

all: make_dir $(NAME)

test:
	$(CXX) $(INCLUDE_AND_LIBS) -o test_game_of_life \
		src/GameOfLife.cpp \
		src/test_GameOfLife.cpp \
		src/CLI.cpp \
		$(FLAGS) -lgtest

make_dir:
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(FLAGS) $(INCLUDE_AND_LIBS) -o $@ -c $<

$(NAME): $(OBJ) $(SRC)
	$(CXX) $(INCLUDE_AND_LIBS) -o $(NAME) $(OBJ) $(FLAGS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all
