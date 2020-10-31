#ifndef MPI_GAME_OF_LIFE_HPP
#define MPI_GAME_OF_LIFE_HPP

#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>

#define CELLMAP_HEIGHT  25
#define CELLMAP_WIDTH   80

class MPI_GameOfLife
{
    typedef std::unique_ptr<std::vector<uint8_t>> map_ptr;

private:
    map_ptr map;
    map_ptr tmp_map;
    unsigned int length;
    unsigned int height;
    unsigned int width;
    unsigned int iteration;

    // For Serial version
    void setCell(unsigned int x, unsigned int y);
    void clearCell(unsigned int x, unsigned int y);

public:
    // TODO: move, copy constructors
    GameOfLife(unsigned int height, unsigned int width);
    GameOfLife(unsigned int height, unsigned int width, const std::vector<uint8_t> &map);
    ~GameOfLife();

    void printMap();
    void makeNextGeneration();
    void randomFillMap();
    unsigned int getIteration() const;
    std::string getDump() const;
};

#endif MPI_GAME_OF_LIFE_HPP
