#include <iostream>
#include <unistd.h>
#include "GameOfLife.h"

int main(int argc, char **argv) {
    GameOfLife game(25, 80);

    game.randomFillMap();

    std::cout << "Start" << std::endl;
    game.printMap();

    for (int i = 0; i < 100000; i++) {
        game.makeNextGeneration();
        std::cout << "Next Generation" << std::endl;
        game.printMap();

        usleep(10000);
    }

    return (0);
}
