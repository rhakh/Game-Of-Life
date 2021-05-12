#ifndef AGAME_OF_LIFE_HPP
#define AGAME_OF_LIFE_HPP

#include <stdint.h>
#include <memory>
#include <vector>

typedef std::vector<std::vector<uint8_t>> Map;
typedef std::unique_ptr<std::vector<std::vector<uint8_t>>> Map_ptr;

enum GOF_verbose_lvl {
    NO_PRINTS,
    EACH_GENERATION,
    LAST_GENERATION
};

class AGameOfLife
{
protected:
    virtual void setCell(int x, int y) = 0;
    virtual void clearCell(int x, int y) = 0;
    virtual int getLiveNeighbours(int x, int y) = 0;
    virtual void randomFillMap() = 0;
    virtual void printMap() = 0;

public:
    // TODO: move, copy constructors

    virtual ~AGameOfLife() {}
    virtual Map_ptr liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose) = 0;
};

#endif // AGAME_OF_LIFE_HPP
