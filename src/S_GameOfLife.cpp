#include <sstream>
#include <thread>
#include <algorithm>
#include <sstream>
#include <chrono>
#include "S_GameOfLife.hpp"

S_GameOfLife::S_GameOfLife(const Map &map)
    : height(map.size()), width(map[0].size()), iteration(0)
{
    this->mMap = std::make_unique<Map>(this->height);
    this->mTmp_map = std::make_unique<Map>(this->height);

    for (int i = 0; i < height; i++) {
        (*this->mMap)[i] = std::vector<uint8_t>(width);
        (*this->mTmp_map)[i] = std::vector<uint8_t>(width);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 1)
                setCell(x, y);
        }
    }
}

S_GameOfLife::S_GameOfLife(int height, int width)
    : height(height), width(width), iteration(0)
{
    this->mMap = std::make_unique<Map>(this->height);
    this->mTmp_map = std::make_unique<Map>(this->height);

    for (int i = 0; i < height; i++) {
        (*this->mMap)[i] = std::vector<uint8_t>(width);
        (*this->mTmp_map)[i] = std::vector<uint8_t>(width);
    }

    randomFillMap();
}

S_GameOfLife::~S_GameOfLife() {}

void S_GameOfLife::printMap() {
    std::stringstream ss;

    ss << "Generation number = " << iteration << std::endl;
    for (int y = 0; y < height; y++) {
        ss << "[" << y << "]\t";
        for (int x = 0; x < width; x++) {
            if (((*mMap)[y][x] & 0x01) == 1)
                ss << "*";
            else
                ss << ".";
        }
        ss << std::endl;
    }

    std::cout << ss.str();
}

void S_GameOfLife::liveOneGeneration() {
    int live_cells_cnt;

    // safe copy to mTmp_map
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            (*mTmp_map)[y][x] = (*mMap)[y][x];

    this->iteration++;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if ((*mTmp_map)[y][x] == 0)
                continue;

            // Get count of alive neightbour cells
            int live_cells_cnt = (*mTmp_map)[y][x] >> 1;

            // Is alive ?
            if ((*mTmp_map)[y][x] & 0x01) {
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

Map_ptr S_GameOfLife::liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose)
{
    auto start_time = std::chrono::high_resolution_clock::now();

    while (num_of_generations > 0) {
        liveOneGeneration();
        if (verbose == EACH_GENERATION)
            printMap();
        num_of_generations--;
    }

    if (verbose == LAST_GENERATION)
        printMap();

    auto ret = std::make_unique<Map>(this->height);
    for (int i = 0; i < height; i++)
        (*ret)[i] = std::vector<uint8_t>(width);

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            (*ret)[y][x] = (*mMap)[y][x];

    auto end_time = std::chrono::high_resolution_clock::now();
    auto spend_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Spend time: " << spend_time.count() << " ms" << std::endl;

    return ret;
}

void S_GameOfLife::randomFillMap() {
    unsigned seed;
    int half_len, x, y;

	// Get seed; random if 0
	seed = (unsigned)time(NULL);
	srand(seed);

	// Randomly initialise cell map with ~50% on pixels
    for (int half_len = (height * width) / 2; half_len > 0; half_len--) {
        x = rand() % (width - 1);
		y = rand() % (height - 1);

        if (((*mMap)[y][x] & 0x01) == 0) {
            setCell(x, y);
        }
    }
}

void S_GameOfLife::setCell(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = (y == 0) ? height - 1 : y - 1;
    y_down = (y == height - 1) ? 0 : y + 1;

    (*mMap)[y][x] |= 0x01;

    (*mMap)[y_up  ][x_left ] += 0x02;
    (*mMap)[y_up  ][x      ] += 0x02;
    (*mMap)[y_up  ][x_right] += 0x02;
    (*mMap)[y     ][x_left ] += 0x02;
    (*mMap)[y     ][x_right] += 0x02;
    (*mMap)[y_down][x_left ] += 0x02;
    (*mMap)[y_down][x      ] += 0x02;
    (*mMap)[y_down][x_right] += 0x02;
}

void S_GameOfLife::clearCell(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = (y == 0) ? height - 1 : y - 1;
    y_down = (y == height - 1) ? 0 : y + 1;

    (*mMap)[y][x] &= ~0x01;

    (*mMap)[y_up  ][x_left ] -= 0x02;
    (*mMap)[y_up  ][x      ] -= 0x02;
    (*mMap)[y_up  ][x_right] -= 0x02;
    (*mMap)[y     ][x_left ] -= 0x02;
    (*mMap)[y     ][x_right] -= 0x02;
    (*mMap)[y_down][x_left ] -= 0x02;
    (*mMap)[y_down][x      ] -= 0x02;
    (*mMap)[y_down][x_right] -= 0x02;
}

int S_GameOfLife::getLiveNeighbours(int x, int y) {
    return ((*mMap)[y][x] >> 1);
}
