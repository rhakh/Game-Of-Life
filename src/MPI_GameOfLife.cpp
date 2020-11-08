#include <sstream>
#include <thread>
#include <algorithm>
#include <sstream>
// #include <mpi.h>
#include "MPI_GameOfLife.hpp"

MPI_GameOfLife::MPI_GameOfLife(unsigned int height, unsigned int width, const std::vector<std::vector<uint8_t>> &map)
    : height(height), width(width), iteration(0)
{
    this->map = std::make_unique<std::vector<std::vector<uint8_t>>>(this->height);
    this->tmp_map = std::make_unique<std::vector<std::vector<uint8_t>>>(this->height);

    for (int i = 0; i < height; i++) {
        (*this->map)[i].emplace_back(width);
        (*this->tmp_map)[i].emplace_back(width);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 1)
                setCell(x, y);
        }
    }
}

MPI_GameOfLife::MPI_GameOfLife(unsigned int height, unsigned int width)
    : height(height), width(width), iteration(0)
{
    // TODO
    // this->map = std::make_unique<map_ptr>(this->length);
    // this->tmp_map = std::make_unique<map_ptr>(this->length);
    // randomFillMap();
}

MPI_GameOfLife::~MPI_GameOfLife() {}

void MPI_GameOfLife::printMap() {
    std::stringstream ss;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (((*map)[y][x] & 0x01) == 1)
                ss << "*";
            else
                ss << ".";
        }
        ss << std::endl;
    }

    std::cout << ss.str();
}

void MPI_GameOfLife::makeNextGeneration() {
    // TODO, change for memcpy
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            (*tmp_map)[y][x] = (*map)[y][x];

    this->iteration++;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int point = (y * width) + x;

            if ((*tmp_map)[y][x] == 0)
                continue;

            // Get count of alive neightbour cells
            unsigned live_cells_cnt = (*tmp_map)[y][x] >> 1;

            // Is alive ?
            if ((*tmp_map)[y][x] & 0x01) {
                // Overpopulation or underpopulation - than die
                if (live_cells_cnt != 2 && live_cells_cnt != 3)
                    clearCell(x, y);
            } else if (live_cells_cnt == 3) {
                // Has 3 neighbours - than revive
                setCell(x, y);
            }
        }
    }
}

void MPI_GameOfLife::randomFillMap() {
    unsigned int x, y, seed;
    int half_len;

	// Get seed; random if 0
	seed = (unsigned)time(NULL);

	// Randomly initialise cell map with ~50% on pixels
	std::cout << "Initializing" << std::endl;

	srand(seed);

	half_len = width * height;
    while (half_len > 0)
    {
        x = rand() % (width - 1);
		y = rand() % (height - 1);

        if (((*map)[y][x] & 0x01) == 0) {
            setCell(x, y);
        }
        
        half_len--;
    }
}

void MPI_GameOfLife::setCell(unsigned int x, unsigned int y) {
    (*map)[y][x] |= 0x1;

    (*map)[y - 1][x - 1] += 0x2;
    (*map)[y - 1][x - 0] += 0x2;
    (*map)[y - 1][x + 1] += 0x2;

    (*map)[y - 0][x - 1] += 0x2;
    (*map)[y - 0][x + 1] += 0x2;

    (*map)[y + 1][x - 1] += 0x2;
    (*map)[y + 1][x - 0] += 0x2;
    (*map)[y + 1][x + 1] += 0x2;
}

void MPI_GameOfLife::clearCell(unsigned int x, unsigned int y) {
    (*map)[y][x] &= ~0x1;

    (*map)[y - 1][x - 1] -= 0x2;
    (*map)[y - 1][x - 0] -= 0x2;
    (*map)[y - 1][x + 1] -= 0x2;

    (*map)[y - 0][x - 1] -= 0x2;
    (*map)[y - 0][x + 1] -= 0x2;

    (*map)[y + 1][x - 1] -= 0x2;
    (*map)[y + 1][x - 0] -= 0x2;
    (*map)[y + 1][x + 1] -= 0x2;
}

unsigned int MPI_GameOfLife::getIteration() const {
    return (this->iteration);
}

std::string MPI_GameOfLife::getDump() const {
    std::stringstream ss;

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            if ((*map)[y][x] & 0x01)
                ss << "*";
            else
                ss << ".";
        }
        ss << std::endl;
    }

    return ss.str();
}
