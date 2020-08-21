#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>

#define CELLMAP_HEIGHT  100
#define CELLMAP_WIDTH   100

class GameOfLife
{
    typedef std::unique_ptr<std::vector<uint8_t>> map_ptr;

private:
    map_ptr map;
    map_ptr tmp_map;
    unsigned int length;
    unsigned int height;
    unsigned int width;

    void setCell(unsigned int x, unsigned int y);
    void clearCell(unsigned int x, unsigned int y);

public:
    GameOfLife(unsigned int height, unsigned int width);
    ~GameOfLife();

    void printMap();
    void makeNextGeneration();
    void randomFillMap();
};
