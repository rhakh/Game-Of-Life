SERIAL_EXE = game_of_life_serial
CONCURRENT_EXE = game_of_life_concurrent
MPI_EXE = game_of_life_mpi

OS := $(shell uname)
ifeq ($(OS),Darwin)
  CXX=mpic++
  INCLUDE_AND_LIBS = -I include \
					-I /usr/local/Cellar/boost/1.72.0_3/include \
					-L /usr/local/Cellar/boost/1.72.0_3/lib
  FLAGS = -std=c++14 -Wall -Wextra -Werror -O2 \
			-Wno-unused -Wno-unused-parameter -Wno-unused-result -Wno-unused-command-line-argument \
			-lboost_filesystem  -lboost_system  -lboost_program_options \
			-pthread -lboost_thread-mt
else
  CXX=mpic++
  INCLUDE_AND_LIBS = -I include \
					-L /usr/lib/x86_64-linux-gnu \
					-I /usr/include/boost
  FLAGS = -std=c++14 -Wall -Wextra -O2 \
			-Wno-unused -Wno-unused-parameter -Wno-unused-result -Wno-unused-command-line-argument \
			-lboost_filesystem  -lboost_system -lboost_program_options \
			-pthread -lboost_thread
			# -Werror TODO
endif


OBJDIR = obj/

SRCDIR = src/

_SRC = 	main.cpp \
		S_GameOfLife.cpp \
		C_GameOfLife.cpp \
		MPI_GameOfLife.cpp \
		CLI.cpp \

SRC = $(addprefix $(SRCDIR), $(_SRC))

OBJ = $(addprefix $(OBJDIR),$(_SRC:.cpp=.o))

all: make_dir $(SERIAL_EXE) $(CONCURRENT_EXE) $(MPI_EXE)

serial: make_dir $(SERIAL_EXE)

concurrent: make_dir $(CONCURRENT_EXE)

mpi: make_dir $(MPI_EXE)

# TODO
test: src/GameOfLife.cpp src/test_GameOfLife.cpp src/CLI.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o test_game_of_life \
		src/GameOfLife.cpp \
		src/test_GameOfLife.cpp \
		src/CLI.cpp \
		$(FLAGS) -lgtest

make_dir:
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(FLAGS) $(INCLUDE_AND_LIBS) -o $@ -c $<

$(SERIAL_EXE): $(OBJ) $(SRC)
	$(CXX) $(INCLUDE_AND_LIBS) -DSERIAL_GOF=1 -DCONCURRENT_GOF=0 -DMPI_GOF=0 -o obj/main.o -c src/main.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o $(SERIAL_EXE) $(OBJ) $(FLAGS)

$(CONCURRENT_EXE): $(OBJ) $(SRC)
	$(CXX) $(INCLUDE_AND_LIBS) -DSERIAL_GOF=0 -DCONCURRENT_GOF=1 -DMPI_GOF=0 -o obj/main.o -c src/main.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o $(CONCURRENT_EXE) $(OBJ) $(FLAGS)

$(MPI_EXE): $(OBJ) $(SRC)
	$(CXX) $(INCLUDE_AND_LIBS) -DSERIAL_GOF=0 -DCONCURRENT_GOF=0 -DMPI_GOF=1 -o obj/main.o -c src/main.cpp
	$(CXX) $(INCLUDE_AND_LIBS) -o $(MPI_EXE) $(OBJ) $(FLAGS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(SERIAL_EXE) $(CONCURRENT_EXE) $(MPI_EXE)

re: fclean all
