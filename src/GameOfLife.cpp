#include <sstream>
#include <thread>
#include <algorithm>
#include "GameOfLife.hpp"

GameOfLife::GameOfLife(unsigned int height, unsigned int width, const std::vector<uint8_t> &map)
    : height(height), width(width), iteration(0)
{
    this->length = height * width;
    this->map = std::make_unique<std::vector<uint8_t>>(this->length);
    this->tmp_map = std::make_unique<std::vector<uint8_t>>(this->length);

    auto it = map.begin();
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            if (*it == 1)
                setCell(x, y);
            it++;
        }
    }
}

GameOfLife::GameOfLife(unsigned int height, unsigned int width)
    : height(height), width(width), iteration(0)
{
    this->length = height * width;
    this->map = std::make_unique<std::vector<uint8_t>>(this->length);
    this->tmp_map = std::make_unique<std::vector<uint8_t>>(this->length);
    randomFillMap();
}

GameOfLife::~GameOfLife() {}

void GameOfLife::printMap() {
    std::stringstream ss;
    auto &cell_map = *(this->map);

    for (unsigned int i = 0; i < this->length; i++) {
        if ((cell_map[i] & 0x01) == 1)
            ss << "*";
        else
            ss << ".";

        if ((i + 1) % width == 0)
            ss << std::endl;
    }

    std::cout << ss.str();
}

void GameOfLife::makeNextGeneration() {
    // TODO: use tmp_map instead from cell_map
    std::vector<uint8_t> cell_map(*(this->map));

    this->iteration++;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int point = (y * width) + x;

            if (cell_map[point] == 0)
                continue;

            // Get count of alive neightbour cells
            unsigned live_cells_cnt = cell_map[point] >> 1;

            // Is alive ?
            if (cell_map[point] & 0x01) {
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

void GameOfLife::randomFillMap() {
    auto &cell_map = *(this->map);
    unsigned int x, y, seed;
    int half_len;

	// Get seed; random if 0
	seed = (unsigned)time(NULL);

	// Randomly initialise cell map with ~50% on pixels
	std::cout << "Initializing" << std::endl;

	srand(seed);

	half_len = length / 2;
    while (half_len > 0)
    {
        x = rand() % (width - 1);
		y = rand() % (height - 1);

        if ((cell_map[(y * width) + x] & 0x01) == 0) {
            setCell(x, y);
        }
        
        half_len--;
    }
}

void GameOfLife::setCell(unsigned int x, unsigned int y) {
    auto &cell_map = *(this->map);
    int point = (y * this->width) + x;
    int left, right, top, bottom;

    left = (x == 0) ? width - 1 : -1;
    right = (x == width - 1) ? -(width - 1) : 1;
    top = (y == 0) ? length - width : -width;
    bottom = (y == height - 1) ? -(length - width) : width;

    // Set cell as alive
    cell_map[point] |= 0x01;

    // Increment live cells count in neighbours cells
    cell_map[point + left] += 0x02;
    cell_map[point + left + top] += 0x02;
    cell_map[point + top] += 0x02;
    cell_map[point + top + right] += 0x02;
    cell_map[point + right] += 0x02;
    cell_map[point + right + bottom] += 0x02;
    cell_map[point + bottom] += 0x02;
    cell_map[point + bottom + left] += 0x02;
}

void GameOfLife::clearCell(unsigned int x, unsigned int y) {
    auto &cell_map = *(this->map);
    int point = (y * this->width) + x;
    int left, right, top, bottom;

    left = (x == 0) ? width - 1 : -1;
    right = (x == width - 1) ? -(width - 1) : 1;
    top = (y == 0) ? length - width : -width;
    bottom = (y == height - 1) ? -(length - width) : width;

    // Set cell as dead
    cell_map[point] &= ~0x01;

    // Decrement live cells count in neighbours cells
    cell_map[point + left] -= 0x02;
    cell_map[point + left + top] -= 0x02;
    cell_map[point + top] -= 0x02;
    cell_map[point + top + right] -= 0x02;
    cell_map[point + right] -= 0x02;
    cell_map[point + right + bottom] -= 0x02;
    cell_map[point + bottom] -= 0x02;
    cell_map[point + bottom + left] -= 0x02;
}

unsigned int GameOfLife::getIteration() const {
    return (this->iteration);
}

unsigned int GameOfLife::getLiveNeighbors(unsigned int x, unsigned int y, const std::vector<uint8_t> &cell_map) {
    unsigned int liveNeighbors;
    int point = (y * this->width) + x;
    int left, right, top, bottom;

    left = (x == 0) ? width - 1 : -1;
    right = (x == width - 1) ? -(width - 1) : 1;
    top = (y == 0) ? length - width : -width;
    bottom = (y == height - 1) ? -(length - width) : width;

    liveNeighbors = (cell_map[point + left] & 0x01) +
                    (cell_map[point + left + top] & 0x01) +
                    (cell_map[point + top] & 0x01) +
                    (cell_map[point + top + right] & 0x01) +
                    (cell_map[point + right] & 0x01) +
                    (cell_map[point + right + bottom] & 0x01) +
                    (cell_map[point + bottom] & 0x01) +
                    (cell_map[point + bottom + left] & 0x01);
    
    return liveNeighbors;
}

void GameOfLife::setCellRawState(unsigned int x, unsigned int y, unsigned int state, unsigned int neighbors)  {
    auto &cell_map = *(this->map);
    int point = (y * this->width) + x;

    // Is alive ?
    if (state) {
        // Overpopulation or underpopulation - than die
        if (neighbors != 2 && neighbors != 3)
            cell_map[point] = (neighbors << 1);
    } else if (neighbors == 3) {
        // Has 3 neighbours - than revive
        cell_map[point] = (neighbors << 1) | 0x01;
    }
}

void GameOfLife::makeNextGenerationConcurrent() {
    auto &cell_map = *(this->tmp_map);
    std::vector<std::thread> threads(std::thread::hardware_concurrency());
    auto process_line = [this, &cell_map](unsigned int y_start, unsigned int y_end) {
        for (unsigned int y = y_start; y < y_end; y++) {
            for (unsigned int x = 0; x < width; x++) {
                unsigned int point = (y * width) + x;
                auto liveNeighbors = getLiveNeighbors(x, y, cell_map);
                setCellRawState(x, y, cell_map[point] & 0x01, liveNeighbors);
            }
        }
    };

    // Copy original map to tmp
    *tmp_map = *map;

    this->iteration++;
    auto cores = std::thread::hardware_concurrency();
    unsigned int step = height / cores;
    for (unsigned int y = 0; y < height; y += step) {
        if (y + step < height)
            threads.emplace_back(process_line, y, y + step);
        else
            threads.emplace_back(process_line, y, height);
    }

    for (auto &thread : threads) {
        if (thread.joinable())
            thread.join();
    }
}
