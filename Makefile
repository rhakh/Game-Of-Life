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
					# -Werror
  FLAGS = -std=c++14 -Wall -Wextra  \
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

test: src/GameOfLife.cpp src/test_GameOfLife.cpp src/CLI.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o test_game_of_life \
		src/GameOfLife.cpp \
		src/test_GameOfLife.cpp \
		src/CLI.cpp \
		$(FLAGS) -lgtest

make_dir:
	mkdir -p $(OBJDIR)

#========== MPI ==========
mpi: obj/MPI_GameOfLife.o obj/MPI_main.o src/MPI_GameOfLife.cpp src/MPI_main.cpp
	$(CXX) -o mpi_game_of_life obj/MPI_GameOfLife.o obj/MPI_main.o $(FLAGS)

obj/MPI_GameOfLife.o: src/MPI_GameOfLife.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o obj/MPI_GameOfLife.o -c src/MPI_GameOfLife.cpp $(FLAGS)

obj/MPI_main.o: src/MPI_main.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o obj/MPI_main.o -c src/MPI_main.cpp $(FLAGS)

mpi_clean:
	rm -f obj/MPI_GameOfLife.o obj/MPI_main.o mpi_game_of_life
# ==========    ==========

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(FLAGS) $(INCLUDE_AND_LIBS) -o $@ -c $<

$(NAME): $(OBJ) $(SRC)
	$(CXX) $(INCLUDE_AND_LIBS) -o $(NAME) $(OBJ) $(FLAGS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all
