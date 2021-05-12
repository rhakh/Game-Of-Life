#include <sstream>
#include <thread>
#include <algorithm>
#include <sstream>
#include "C_GameOfLife.hpp"

C_GameOfLife::C_GameOfLife(const Map &map)
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

C_GameOfLife::C_GameOfLife(int height, int width)
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

C_GameOfLife::~C_GameOfLife() {}

void C_GameOfLife::printMap() {
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

void C_GameOfLife::randomFillMap() {
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

int C_GameOfLife::getLiveNeighbours(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = (y == 0) ? height - 1 : y - 1;
    y_down = (y == height - 1) ? 0 : y + 1;

    return  (*mTmp_map)[y_up  ][x_left ] + (*mTmp_map)[y_up  ][x      ] +
            (*mTmp_map)[y_up  ][x_right] + (*mTmp_map)[y     ][x_left ] +
            (*mTmp_map)[y     ][x_right] + (*mTmp_map)[y_down][x_left ] +
            (*mTmp_map)[y_down][x      ] + (*mTmp_map)[y_down][x_right];
}

void inline C_GameOfLife::setCell(int x, int y) {
    (*mMap)[y][x] = 1;
}

void inline C_GameOfLife::clearCell(int x, int y) {
    (*mMap)[y][x] = 0;
}

int C_GameOfLife::getChunkBeginning(int my_rank, int total_num_proc) {
    int chunk_size = height / total_num_proc;
    int rest = height % total_num_proc;
    int beginning = chunk_size * my_rank;

    if (rest != 0) {
        if (my_rank == 0)
            return beginning;
        if (my_rank >= rest)
            return beginning + rest;
        beginning += my_rank;
    }
    return beginning;
}

int C_GameOfLife::getChunkEnd(int my_rank, int total_num_proc) {
    int next_begining = getChunkBeginning(my_rank + 1, total_num_proc);
    return std::min(height, next_begining);
}

void C_GameOfLife::liveOneGeneration() {
    auto &map = *(this->mMap);
    auto &tmp_map = *(this->mTmp_map);
    std::vector<std::thread> threads(std::thread::hardware_concurrency());
    auto process_line = [this, &map, &tmp_map](int y_start, int y_end) {
        for (int y = y_start; y < y_end; y++) {
            for (int x = 0; x < width; x++) {
                auto liveNeighbors = getLiveNeighbours(x, y);

                // Is alive ?
                if (tmp_map[y][x] & 0x01) {
                    // Overpopulation or underpopulation - than die
                    if (liveNeighbors != 2 && liveNeighbors != 3)
                        clearCell(x, y);
                } else if (liveNeighbors == 3) {
                    // Has 3 neighbours - than revive
                    setCell(x, y);
                }
            }
        }
    };

    // Copy original map to tmp
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            (*mTmp_map)[y][x] = (*mMap)[y][x];

    this->iteration++;
    int cores = std::thread::hardware_concurrency();
    for (int i = 0; i < cores; i++)
        threads.emplace_back(process_line, getChunkBeginning(i, cores), getChunkEnd(i, cores));

    for (auto &thread : threads) {
        if (thread.joinable())
            thread.join();
    }
}

Map_ptr C_GameOfLife::liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose)
{
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_of_generations; i++) {
        liveOneGeneration();
        if (verbose == EACH_GENERATION)
            printMap();
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
