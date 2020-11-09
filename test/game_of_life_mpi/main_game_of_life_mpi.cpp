#include <vector>
#include <iostream>
#include <assert.h>
#include <mpi.h>

#include "game_of_life.hpp"
#include "test_helpers.hpp"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

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

void print_grid(Grid &grid, std::string name) {
    std::cout << "--- Print " << name << " grid:" << std::endl;
    for (int y = 0; y < grid[0].size(); y++) {
        for (int x = 0; x < grid.size(); x++) {
            std::cout << (int)(grid[x][y] & 0x01) << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv)
{
    signal(SIGSEGV, handler);

    MPI_Init(&argc, &argv);
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    Grid conways_pentadecathlon_state_11 = grid_int_to_grid({
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
    Grid conways_pentadecathlon_state_7 = grid_int_to_grid({
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                });
    int NUMBER_OF_TIMES_TO_ADD_HALO = 100;
    Grid huge_conways_pentadecathlon_state_11 = add_halo_n_times(
        conways_pentadecathlon_state_11,
        NUMBER_OF_TIMES_TO_ADD_HALO);
    Grid huge_conways_pentadecathlon_state_7 = add_halo_n_times(
        conways_pentadecathlon_state_7,
        NUMBER_OF_TIMES_TO_ADD_HALO);
    // Grid result = tick_grid_n_times(conways_pentadecathlon_state_11, 26);
    // Grid result_huge = tick_grid_n_times(huge_conways_pentadecathlon_state_11, 26);

    // if (my_rank == 0) {
    //     print_grid(conways_pentadecathlon_state_11, "conways_pentadecathlon_state_11");
    //     print_grid(result, "result");
    //     print_grid(conways_pentadecathlon_state_7, "conways_pentadecathlon_state_7");
    // }

    {
        Grid result = tick_grid_n_times(conways_pentadecathlon_state_11, 2);

        if (my_rank == 0) {
            print_grid(result, "result");
        }
    }

    // if (my_rank == 0) {
    //     assert( result == conways_pentadecathlon_state_7 );
    //     assert( result_huge == huge_conways_pentadecathlon_state_7 );
    //     cout << "results match expectations" << endl;
    // }

    std::cout << "Finalize " << std::endl;

    MPI_Finalize();
}