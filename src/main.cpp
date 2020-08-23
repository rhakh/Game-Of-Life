#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "GameOfLife.hpp"
#include "CLI.hpp"

// TODO: remove this when move constructor will be done
void randomGame(CLI &cli) {
    GameOfLife game(cli.getHeight(), cli.getWidth());

    for (int i = 0; i < 100000; i++) {
        std::cout << "Generation: " << game.getIteration() << " iteration" << std::endl;
        game.printMap();
        game.makeNextGeneration();
        usleep(250000);
        std::system("clear");
    }
}

int main(int argc, char **argv) {
    CLI cli(argc, argv);

    if (cli.isFlagSet("help"))
        return (0);

    if (!cli.isFlagSet("file")) {
        randomGame(cli);
        return (0);
    }

    GameOfLife game(cli.getHeight(), cli.getWidth(), cli.getMap());

    for (int i = 0; i < 100000; i++) {
        std::cout << "Generation: " << game.getIteration() << " iteration" << std::endl;
        game.printMap();
        game.makeNextGeneration();
        usleep(25000);
        std::system("clear");
    }

    return (0);
}
