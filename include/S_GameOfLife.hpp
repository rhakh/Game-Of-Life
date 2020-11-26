#ifndef SERIAL_GAME_OF_LIFE_HPP
#define SERIAL_GAME_OF_LIFE_HPP

// Serial implementation of Game Of Life

#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>
#include "AGameOfLife.hpp"

class S_GameOfLife : public AGameOfLife {
private:
    Map_ptr mMap;
    Map_ptr mTmp_map;
    int height;
    int width;
    int iteration;

    void setCell(int x, int y);
    void clearCell(int x, int y);
    int getLiveNeighbours(int x, int y);
    void randomFillMap();
    void printMap();
    void liveOneGeneration();

public:
    // TODO: move, copy constructors
    S_GameOfLife(int height, int width);
    S_GameOfLife(const Map &map);
    ~S_GameOfLife();

    Map_ptr liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose);
};

#endif // SERIAL_GAME_OF_LIFE_HPP
