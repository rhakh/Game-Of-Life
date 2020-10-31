CC=mpic++
CFLAGS=-Wall -Wextra -Wpedantic -std=c++14 -O2

main_game_of_life_mpi: main_game_of_life_mpi.o game_of_life.o test_helpers.o
	$(CC) -o main_game_of_life_mpi main_game_of_life_mpi.o game_of_life.o test_helpers.o $(CFLAGS)

test: catch.o test_helpers.o game_of_life.o test_game_of_life.cpp
	$(CC) -o test catch.o test_game_of_life.cpp game_of_life.o test_helpers.o $(CFLAGS)

main_game_of_life_mpi.o: main_game_of_life_mpi.cpp
	$(CC) -o main_game_of_life_mpi.o -c main_game_of_life_mpi.cpp $(CFLAGS)

game_of_life.o: game_of_life.hpp game_of_life.cpp test_helpers.o
	$(CC) -o game_of_life.o -c game_of_life.cpp $(CFLAGS)

catch.o: catch.cpp
	$(CC) -o catch.o -c catch.cpp $(CFLAGS)

test_helpers.o: test_helpers.cpp test_helpers.hpp
	$(CC) -o test_helpers.o -c test_helpers.cpp $(CFLAGS)

clean:
	rm -f *.o main_game_of_life_mpi test
