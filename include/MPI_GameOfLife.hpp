#ifndef MPI_GAME_OF_LIFE_HPP
#define MPI_GAME_OF_LIFE_HPP

// MPI implementation of Game Of Life

#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>
#include "AGameOfLife.hpp"

class MPI_GameOfLife : public AGameOfLife {
private:
    Map_ptr mMap;
    Map_ptr mTmp_map;
    int height;
    int width;
    int iteration;

    int getChunkBeginning(int my_rank, int total_num_proc);
    int getChunkEnd(int my_rank, int total_num_proc);
    void liveOneGeneration(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc);
    void sendAndRecvLine(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc);
    void sendAndRecvLineForBorders(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc);
    void _liveNGeneration(int my_rank, int total_num_proc, int num_of_generations, GOF_verbose_lvl verbose);

    void setCell(int x, int y);
    void clearCell(int x, int y);
    int getLiveNeighbours(int x, int y);
    void randomFillMap();
    void printMap();

public:
    // TODO: move, copy constructors
    MPI_GameOfLife(int height, int width);
    MPI_GameOfLife(const Map &map);
    ~MPI_GameOfLife();

    Map_ptr liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose);
};

#endif // MPI_GAME_OF_LIFE_HPP
