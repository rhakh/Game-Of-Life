#ifndef MPI_GAME_OF_LIFE_HPP
#define MPI_GAME_OF_LIFE_HPP

#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>

class MPI_GameOfLife
{
    typedef std::unique_ptr<std::vector<std::vector<uint8_t>>> map_ptr;

private:
    map_ptr map;
    map_ptr tmp_map;
    int height;
    int width;
    unsigned int iteration;
    void inline setCell(unsigned int x, unsigned int y);
    void inline clearCell(unsigned int x, unsigned int y);

public:
    // TODO: move, copy constructors
    MPI_GameOfLife(unsigned int height, unsigned int width);
    MPI_GameOfLife(const std::vector<std::vector<uint8_t>> &map);
    ~MPI_GameOfLife();

    void printMap();
    void randomFillMap();
    unsigned int getIteration() const;
    std::string getDump() const;

    int get_chunk_beginning(int my_rank, int total_num_proc);
    int get_chunk_end(int my_rank, int total_num_proc);
    int inline get_live_neighbours(int x, int y);
    void liveGeneration(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc);
    void sendAndRecvLine(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc);
    void sendAndRecvLineForBorders(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc);
    void liveNGeneration(int my_rank, int total_num_proc, int num_of_generations);

};

#endif // MPI_GAME_OF_LIFE_HPP
