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

int main(int argc, const char **argv) {
    CLI cli(argc, argv);
    std::string dump_s;
    unsigned int dump = 0;
    bool verbose = false, concurrent = false;

    if (cli.isFlagSet("help"))
        return (0);

    if (!cli.isFlagSet("file")) {
        randomGame(cli);
        return (0);
    }

    if (cli.isFlagSet("verbose"))
        verbose = true;

    if (cli.isFlagSet("concurrent"))
        concurrent = true;

    if (cli.getFlag("dump", dump_s))
        dump = std::atoi(dump_s.c_str());

    GameOfLife game(cli.getHeight(), cli.getWidth(), cli.getMap());

    for (unsigned int i = 0; i < 1500; i++) {
        if (verbose) {
            std::cout << "Generation: " << game.getIteration() << " iteration" << std::endl;
            game.printMap();
            usleep(25000);
            std::system("clear");
        }

        if (dump && dump == i) {
            std::cout << "Dump generation " << game.getIteration() << std::endl;
            std::cout << game.getDump() << std::endl;
        }

        if (concurrent)
            game.makeNextGenerationConcurrent();
        else
            game.makeNextGeneration();
    }

    return (0);
}

/* TODO:
libs for compiling
sudo apt-get install libboost-all-dev
sudo apt install libgtest-dev https://medium.com/@rvarago/introduction-to-google-c-unit-testing-3d564c30f3b0

*/