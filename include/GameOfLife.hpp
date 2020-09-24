#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>

#define CELLMAP_HEIGHT  25
#define CELLMAP_WIDTH   80

class GameOfLife
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

    // For Concurrent version
    unsigned int getLiveNeighbors(unsigned int x, unsigned int y, const std::vector<uint8_t> &cell_map);
    void setCellRawState(unsigned int x, unsigned int y, unsigned int state, unsigned int neighbors);

public:
    // TODO: move, copy constructors
    GameOfLife(unsigned int height, unsigned int width);
    GameOfLife(unsigned int height, unsigned int width, const std::vector<uint8_t> &map);
    ~GameOfLife();

    void printMap();
    void makeNextGeneration();
    void makeNextGenerationConcurrent();
    void randomFillMap();
    unsigned int getIteration() const;
};
