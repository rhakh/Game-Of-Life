#ifndef CONCURRENT_GAME_OF_LIFE_HPP
#define CONCURRENT_GAME_OF_LIFE_HPP

// Concurrent implementation of Game Of Life

#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>
#include "AGameOfLife.hpp"

class C_GameOfLife : public AGameOfLife {
private:
    Map_ptr mMap;
    Map_ptr mTmp_map;
    int height;
    int width;
    int iteration;

    int getChunkBeginning(int my_rank, int total_num_proc);
    int getChunkEnd(int my_rank, int total_num_proc);
    void liveOneGeneration();
    void inline setCell(int x, int y);
    void inline clearCell(int x, int y);
    int getLiveNeighbours(int x, int y);
    void randomFillMap();
    void printMap();

public:
    // TODO: move, copy constructors
    C_GameOfLife(int height, int width);
    C_GameOfLife(const Map &map);
    ~C_GameOfLife();

    Map_ptr liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose);
};

#endif // CONCURRENT_GAME_OF_LIFE_HPP
