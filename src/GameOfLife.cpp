#include "GameOfLife.h"

GameOfLife::GameOfLife(unsigned int height = CELLMAP_HEIGHT, unsigned int width = CELLMAP_WIDTH)
    : height(height), width(width)
{
    this->length = height * width;
    this->map = std::make_unique<std::vector<uint8_t>>(this->length);
    this->tmp_map = std::make_unique<std::vector<uint8_t>>(this->length);
}

GameOfLife::~GameOfLife() {}

void GameOfLife::printMap() {
    // TODO: use sstream to construct output, and then print it
    auto &cell_map = *(this->map);

    for (unsigned int i = 0; i < this->length; i++) {
        if ((cell_map[i] & 0x01) == 1)
            std::cout << "*";
        else
            std::cout << ".";

        if ((i + 1) % width == 0)
            std::cout << std::endl;
    }
}

void GameOfLife::makeNextGeneration() {
    std::vector<uint8_t> cell_map(*(this->map));

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
