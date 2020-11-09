#include "MPI_GameOfLife.hpp"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

std::vector<std::vector<uint8_t>> default_map({
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            });

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, const char **argv) {
    signal(SIGSEGV, handler);

    MPI_GameOfLife  game(default_map.size(), default_map[0].size(), default_map);
    std::cout << "Start:" << std::endl;
    game.printMap();

    for (int i = 0; i < 26; i++) {
        game.makeNextGeneration();
    }

    std::cout << "Finish:" << std::endl;
    game.printMap();

    return (0);
}