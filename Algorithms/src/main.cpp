#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "CLI.hpp"
#include "AGameOfLife.hpp"

#if SERIAL_GOF
    #include "S_GameOfLife.hpp"
#elif CONCURRENT_GOF
    #include "C_GameOfLife.hpp"
#elif MPI_GOF
    #include "MPI_GameOfLife.hpp"
#endif

// TODO: remove this when move constructor will be done
void randomGame(CLI &cli) {
    // TODO
    // GameOfLife game(cli.getHeight(), cli.getWidth());

    // for (int i = 0; i < 100000; i++) {
    //     std::cout << "Generation: " << game.getIteration() << " iteration" << std::endl;
    //     game.printMap();
    //     game.makeNextGeneration();
    //     usleep(250000);
    //     std::system("clear");
    // }
}

int main(int argc, char **argv) {
    CLI cli(argc, (const char **)argv);
    std::string num_s, ver_s;
    int num_of_generations = 1500;
    auto verbose = NO_PRINTS;

    if (cli.isFlagSet("help"))
        return (0);

    if (!cli.isFlagSet("file")) {
        randomGame(cli);
        return (0);
    }

    if (cli.getFlag("verbose", ver_s))
        verbose = (GOF_verbose_lvl) std::atoi(ver_s.c_str());

    if (cli.getFlag("num", num_s))
        num_of_generations = std::atoi(num_s.c_str());

#if SERIAL_GOF
    S_GameOfLife game(cli.getMap());
    game.liveNGeneration(argc, argv, num_of_generations, verbose);
#elif CONCURRENT_GOF
    C_GameOfLife game(cli.getMap());
    game.liveNGeneration(argc, argv, num_of_generations, verbose);
#elif MPI_GOF
    MPI_GameOfLife game(cli.getMap());
    game.liveNGeneration(argc, argv, num_of_generations, verbose);
#endif

    return (0);
}

/* TODO:
libs for compiling
sudo apt-get install libboost-all-dev
sudo apt install libgtest-dev https://medium.com/@rvarago/introduction-to-google-c-unit-testing-3d564c30f3b0
sudo apt-get install -y qml-module-qtcharts
*/
