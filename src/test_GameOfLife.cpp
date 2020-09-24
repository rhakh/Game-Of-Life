#include <gtest/gtest.h>
#include "GameOfLife.hpp"
#include  "CLI.hpp"

TEST(TimeTest, sequential) {
    const char *argv[3] = {"name", "-f", "Patterns/puffer-train.txt"};
    int argc = 3;
    CLI cli(argc, argv);
    GameOfLife game(cli.getHeight(), cli.getWidth(), cli.getMap());

    for (int i = 0; i < 1500; i++)
        game.makeNextGeneration();

    SUCCEED();
}

TEST(TimeTest, concurrent) {
    const char *argv[3] = {"name", "-f", "Patterns/puffer-train.txt"};
    int argc = 3;
    CLI cli(argc, argv);
    GameOfLife game(cli.getHeight(), cli.getWidth(), cli.getMap());

    for (int i = 0; i < 1500; i++)
        game.makeNextGenerationConcurrent();

    SUCCEED();
}

TEST(TimeTest, sequentialBig) {
    const char *argv[3] = {"name", "-f", "Patterns/puffer-train-big.txt"};
    int argc = 3;
    CLI cli(argc, argv);
    GameOfLife game(cli.getHeight(), cli.getWidth(), cli.getMap());

    for (int i = 0; i < 5000; i++)
        game.makeNextGeneration();

    SUCCEED();
}

TEST(TimeTest, concurrentBig) {
    const char *argv[3] = {"name", "-f", "Patterns/puffer-train-big.txt"};
    int argc = 3;
    CLI cli(argc, argv);
    GameOfLife game(cli.getHeight(), cli.getWidth(), cli.getMap());

    for (int i = 0; i < 5000; i++)
        game.makeNextGenerationConcurrent();

    SUCCEED();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}