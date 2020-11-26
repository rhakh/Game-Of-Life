#include <sstream>
#include <thread>
#include <algorithm>
#include <sstream>
#include <mpi.h>
#include "MPI_GameOfLife.hpp"

int TAG = 0;  // for MPI communications
int TAG_BORDER = 1;

MPI_GameOfLife::MPI_GameOfLife(const Map &map)
    : height(map.size()), width(map[0].size()), iteration(0)
{
    this->mMap = std::make_unique<Map>(this->height + 2); // 2 = halo
    this->mTmp_map = std::make_unique<Map>(this->height + 2); // 2 = halo

    for (int i = 0; i < height + 2; i++) {
        (*this->mMap)[i] = std::vector<uint8_t>(width);
        (*this->mTmp_map)[i] = std::vector<uint8_t>(width);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 1)
                setCell(x, y + 1); // exclude halo
        }
    }
}

MPI_GameOfLife::MPI_GameOfLife(int height, int width)
    : height(height), width(width), iteration(0)
{
    // Useless in MPI implementation, because map
    // will be different in all ranks
    throw("Cant use random map in MPI implementation");
}

MPI_GameOfLife::~MPI_GameOfLife() {}

void MPI_GameOfLife::printMap() {
    std::stringstream ss;

    ss << "Generation number = " << iteration << std::endl;
    for (int y = 1; y < height + 1; y++) {
        ss << "[" << (y - 1) << "]\t";
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

int MPI_GameOfLife::getChunkBeginning(int my_rank, int total_num_proc) {
    int chunk_size = height / total_num_proc;
    int rest = height % total_num_proc;
    int beginning = chunk_size * my_rank;
    if (rest != 0) {
        if (my_rank == 0)
            return beginning + 1; // + 1 halo
        if (my_rank >= rest)
            return beginning + rest + 1; // + 1 halo
        beginning += my_rank;
    }
    return beginning + 1; // + 1 halo
}

int MPI_GameOfLife::getChunkEnd(int my_rank, int total_num_proc) {
    int next_begining = getChunkBeginning(my_rank + 1, total_num_proc);
    return std::min((int)(height + 1), next_begining);
}

int MPI_GameOfLife::getLiveNeighbours(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = y - 1;
    y_down = y + 1;

    return  (*mTmp_map)[y_up  ][x_left ] + (*mTmp_map)[y_up  ][x      ] +
            (*mTmp_map)[y_up  ][x_right] + (*mTmp_map)[y     ][x_left ] +
            (*mTmp_map)[y     ][x_right] + (*mTmp_map)[y_down][x_left ] +
            (*mTmp_map)[y_down][x      ] + (*mTmp_map)[y_down][x_right];
}

void MPI_GameOfLife::liveOneGeneration(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc) {
    int live_neighbours;

    // copy chunk
    for (int y = (chunk_beginning - 1); y < (chunk_end + 1); y++)
        for (int x = 0; x < width; x++)
            (*mTmp_map)[y][x] = (*mMap)[y][x];

    this->iteration++;
    for (int y = chunk_beginning; y < chunk_end; y++) {
        for (int x = 0; x < width; x++) {
            live_neighbours = getLiveNeighbours(x, y);

            if ((*mTmp_map)[y][x] == 1) {
                if (live_neighbours != 2 && live_neighbours != 3)
                    clearCell(x, y);
            } else if (live_neighbours == 3) {
                setCell(x, y);
            }
        }
    }
}

void MPI_GameOfLife::sendAndRecvLine(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc) {
    bool there_is_chunk_at_top = my_rank != 0;
    if (there_is_chunk_at_top) {
        // Receive from src other side of the top border,
        // save to other_side_border. (1 to skip halo)
        int src = my_rank - 1;  // prev process
        int other_side_border = chunk_beginning - 1;
        MPI_Recv(&(*mMap)[other_side_border][0], width, MPI_UNSIGNED_CHAR, src, TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Send my side of the top border to dest. (1 to skip halo)
        int dest = my_rank - 1; // previous process
        int my_first_line = chunk_beginning;
        MPI_Send(&(*mMap)[my_first_line][0], width, MPI_UNSIGNED_CHAR, dest, TAG,
                 MPI_COMM_WORLD);
    }

    int last_process = total_num_proc - 1;
    bool there_is_chunk_at_bottom = my_rank != last_process;
    if (there_is_chunk_at_bottom) {
        // Send my side of the bottom border to dest. (1 to skip halo)
        int dest = my_rank + 1;  // next process
        int my_last_line = chunk_end - 1;
        MPI_Send(&(*mMap)[my_last_line][0], width, MPI_UNSIGNED_CHAR, dest, TAG,
                 MPI_COMM_WORLD);

        // Receive from src other side of the bottom border,
        // save to other_side_border. (1 to skip halo)
        int src = my_rank + 1;  // next process
        int other_side_border = my_last_line + 1;
        MPI_Recv(&(*mMap)[other_side_border][0], width, MPI_UNSIGNED_CHAR, src, TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void MPI_GameOfLife::sendAndRecvLineForBorders(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc) {
    // Only for 0 and total_num_proc - 1 ranks
    if(my_rank == 0) {
        // Receive from last proc, bottom border,
        // save to [height] line. (chunk_beginning - 1 to recv in halo line)
        int src = total_num_proc - 1;  // last process
        MPI_Recv(&(*mMap)[chunk_beginning - 1][0], width, MPI_UNSIGNED_CHAR, src, TAG_BORDER,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Send top border to last proc.
        int dest = total_num_proc - 1; // last process
        MPI_Send(&(*mMap)[chunk_beginning][0], width, MPI_UNSIGNED_CHAR, dest, TAG_BORDER,
                 MPI_COMM_WORLD);
    }

    if (my_rank == total_num_proc - 1) {
        // Send bottom border to first proc.
        int dest = 0;  // first process
        MPI_Send(&(*mMap)[chunk_end - 1][0], width, MPI_UNSIGNED_CHAR, dest, TAG_BORDER,
                 MPI_COMM_WORLD);

        // Receive from first proc, top border,
        // save to [1] line. (chunk_end to recv in halo line)
        int src = 0;  // next process
        MPI_Recv(&(*mMap)[chunk_end][0], width, MPI_UNSIGNED_CHAR, src, TAG_BORDER,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void MPI_GameOfLife::_liveNGeneration(int my_rank,
                                    int total_num_proc,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose)
{
    int chunk_beginning = getChunkBeginning(my_rank, total_num_proc);
    int chunk_end = getChunkEnd(my_rank, total_num_proc); // end index is not included in chunk

    for (int i = 0; i < num_of_generations; ++i) {
        liveOneGeneration(chunk_beginning, chunk_end, my_rank, total_num_proc);
        if (total_num_proc > 1) {
            sendAndRecvLine(chunk_beginning, chunk_end, my_rank, total_num_proc);
            sendAndRecvLineForBorders(chunk_beginning, chunk_end, my_rank, total_num_proc);
        }
    }

    if (my_rank == 0) {
        // Receive other chunks and merge with the first one
        for(int src = 1; src < total_num_proc; src++) {
            int chunk_beginning = getChunkBeginning(src, total_num_proc);
            int chunk_end = getChunkEnd(src, total_num_proc);
            for(int y = chunk_beginning; y < chunk_end; y++) {
                MPI_Recv(&(*mMap)[y][0], width, MPI_UNSIGNED_SHORT, src, TAG,
                         MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }
    } else {
        // Send chunk to process 0
        int chunk_beginning = getChunkBeginning(my_rank, total_num_proc);
        int chunk_end = getChunkEnd(my_rank, total_num_proc);
        int dest = 0;
        for(int y = chunk_beginning; y < chunk_end; y++) {
            // 1 to skip halo
            MPI_Send(&(*mMap)[y][0], width, MPI_UNSIGNED_SHORT, dest, TAG,
                     MPI_COMM_WORLD);
        }
    }
}

Map_ptr MPI_GameOfLife::liveNGeneration(int argc,
                                    char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose)
{
    int my_rank, total_num_proc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_num_proc);

    if (verbose == EACH_GENERATION) {
        std::cout << "Debug print on each generation are useless in MPI implementation" << std::endl;
    }

    double start_time = MPI_Wtime();
    _liveNGeneration(my_rank, total_num_proc, num_of_generations, verbose);
    double end_time = MPI_Wtime();

    if (my_rank == 0) {
        std::cout << "Spend time: " << (end_time - start_time) * 1000 << " ms" << std::endl;
        if (verbose == LAST_GENERATION)
            printMap();
    }

    MPI_Finalize();

    // Copy mMap and return copy to user
    if (my_rank == 0) {
        auto ret = std::make_unique<Map>(this->height);

        for (int i = 0; i < height; i++)
            (*ret)[i] = std::vector<uint8_t>(width);

        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                (*ret)[y][x] = (*mMap)[y + 1][x];

        return ret;
    }

    return nullptr;
}

void MPI_GameOfLife::randomFillMap() {
    // Useless in MPI implementation, because map
    // will be different in all ranks
    throw("Cant use random map in MPI implementation");
}

void inline MPI_GameOfLife::setCell(int x, int y) {
    (*mMap)[y][x] = 1;
}

void inline MPI_GameOfLife::clearCell(int x, int y) {
    (*mMap)[y][x] = 0;
}
