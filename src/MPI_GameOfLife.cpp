#include <sstream>
#include <thread>
#include <algorithm>
#include <sstream>
#include <mpi.h>
#include "MPI_GameOfLife.hpp"

int TAG = 0;  // for MPI communications
int TAG_BORDER = 1;

MPI_GameOfLife::MPI_GameOfLife(const std::vector<std::vector<uint8_t>> &map)
    : height(map.size()), width(map[0].size()), iteration(0)
{
    this->map = std::make_unique<std::vector<std::vector<uint8_t>>>(this->height + 2); // 2 = halo
    this->tmp_map = std::make_unique<std::vector<std::vector<uint8_t>>>(this->height + 2); // 2 = halo

    for (int i = 0; i < height + 2; i++) {
        (*this->map)[i] = std::vector<uint8_t>(width);
        (*this->tmp_map)[i] = std::vector<uint8_t>(width);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 1)
                setCell(x, y + 1); // exclude halo
        }
    }
}

MPI_GameOfLife::MPI_GameOfLife(unsigned int height, unsigned int width)
    : height(height), width(width), iteration(0)
{
    // TODO
    // this->map = std::make_unique<map_ptr>(this->length);
    // this->tmp_map = std::make_unique<map_ptr>(this->length);
    // randomFillMap();
}

MPI_GameOfLife::~MPI_GameOfLife() {}

void MPI_GameOfLife::printMap() {
    std::stringstream ss;

    for (int y = 1; y < height + 1; y++) {
        ss << "[" << y << "]\t";
        for (int x = 0; x < width; x++) {
            if (((*map)[y][x] & 0x01) == 1)
                ss << "*";
            else
                ss << ".";
        }
        ss << std::endl;
    }

    std::cout << ss.str();
}

int MPI_GameOfLife::get_chunk_beginning(int my_rank, int total_num_proc) {
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

int MPI_GameOfLife::get_chunk_end(int my_rank, int total_num_proc) {
    int next_begining = get_chunk_beginning(my_rank + 1, total_num_proc);
    return std::min((int)(height + 1), next_begining);
}

int inline MPI_GameOfLife::get_live_neighbours(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = y - 1;
    y_down = y + 1;

    return  (*tmp_map)[y_up  ][x_left ] + (*tmp_map)[y_up  ][x      ] +
            (*tmp_map)[y_up  ][x_right] + (*tmp_map)[y     ][x_left ] +
            (*tmp_map)[y     ][x_right] + (*tmp_map)[y_down][x_left ] +
            (*tmp_map)[y_down][x      ] + (*tmp_map)[y_down][x_right];
}

void MPI_GameOfLife::liveGeneration(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc) {
    // TODO, change for memcpy
    int live_neighbours;

    // copy chunk
    for (int y = (chunk_beginning - 1); y < (chunk_end + 1); y++)
        for (int x = 0; x < width; x++)
            (*tmp_map)[y][x] = (*map)[y][x];

    this->iteration++;
    for (int y = chunk_beginning; y < chunk_end; y++) {
        for (int x = 0; x < width; x++) {
            int live_neighbours = get_live_neighbours(x, y);

            if ((*tmp_map)[y][x] == 1) {
                if (live_neighbours != 2 && live_neighbours != 3)
                    clearCell(x, y);
            } else if (live_neighbours == 3) {
                setCell(x, y);
            }
        }
    }
}

std::string get_line(std::vector<uint8_t> &line, int length) {
    std::stringstream ss;

    for (int i = 0; i < length; i++) {
        // if (((*map)[y][x] & 0x01) == 1)
        //     ss << "*";
        // else
        //     ss << ".";
        ss << (int)line[i] << " ";
    }

    return ss.str();
}

void MPI_GameOfLife::sendAndRecvLine(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc) {
    bool there_is_chunk_at_top = my_rank != 0;
    if(there_is_chunk_at_top) {
        // Receive from src other side of the top border,
        // save to other_side_border. (1 to skip halo)
        int src = my_rank - 1;  // prev process
        int other_side_border = chunk_beginning - 1;
        MPI_Recv(&(*map)[other_side_border][0], width, MPI_UNSIGNED_CHAR, src, TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Send my side of the top border to dest. (1 to skip halo)
        int dest = my_rank - 1; // previous process
        int my_first_line = chunk_beginning;
        MPI_Send(&(*map)[my_first_line][0], width, MPI_UNSIGNED_CHAR, dest, TAG,
                 MPI_COMM_WORLD);
    }

    int last_process = total_num_proc - 1;
    bool there_is_chunk_at_bottom = my_rank != last_process;
    if(there_is_chunk_at_bottom) {
        // Send my side of the bottom border to dest. (1 to skip halo)
        int dest = my_rank + 1;  // next process
        int my_last_line = chunk_end - 1;
        MPI_Send(&(*map)[my_last_line][0], width, MPI_UNSIGNED_CHAR, dest, TAG,
                 MPI_COMM_WORLD);

        // Receive from src other side of the bottom border,
        // save to other_side_border. (1 to skip halo)
        int src = my_rank + 1;  // next process
        int other_side_border = my_last_line + 1;
        MPI_Recv(&(*map)[other_side_border][0], width, MPI_UNSIGNED_CHAR, src, TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void MPI_GameOfLife::sendAndRecvLineForBorders(int chunk_beginning, int chunk_end, int my_rank, int total_num_proc) {
    // Only for 0 and total_num_proc - 1 ranks
    if(my_rank == 0) {
        // Receive from last proc, bottom border,
        // save to [height] line. (chunk_beginning - 1 to recv in halo line)
        int src = total_num_proc - 1;  // last process
        MPI_Recv(&(*map)[chunk_beginning - 1][0], width, MPI_UNSIGNED_CHAR, src, TAG_BORDER,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Send top border to last proc.
        int dest = total_num_proc - 1; // last process
        MPI_Send(&(*map)[chunk_beginning][0], width, MPI_UNSIGNED_CHAR, dest, TAG_BORDER,
                 MPI_COMM_WORLD);
    }

    if (my_rank == total_num_proc - 1) {
        // Send bottom border to first proc.
        int dest = 0;  // first process
        MPI_Send(&(*map)[chunk_end - 1][0], width, MPI_UNSIGNED_CHAR, dest, TAG_BORDER,
                 MPI_COMM_WORLD);

        // Receive from first proc, top border,
        // save to [1] line. (chunk_end to recv in halo line)
        int src = 0;  // next process
        MPI_Recv(&(*map)[chunk_end][0], width, MPI_UNSIGNED_CHAR, src, TAG_BORDER,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void MPI_GameOfLife::liveNGeneration(int my_rank, int total_num_proc, int num_of_generations) {
    int chunk_beginning = get_chunk_beginning(my_rank, total_num_proc);
    int chunk_end = get_chunk_end(my_rank, total_num_proc); // end index is not included in chunk

    for (int i = 0; i < num_of_generations; ++i) {
        liveGeneration(chunk_beginning, chunk_end, my_rank, total_num_proc);
        if (total_num_proc > 1) {
            sendAndRecvLine(chunk_beginning, chunk_end, my_rank, total_num_proc);
            sendAndRecvLineForBorders(chunk_beginning, chunk_end, my_rank, total_num_proc);
        }
    }

    if (my_rank == 0) {
        // Receive other chunks and merge with the first one
        for(int src = 1; src < total_num_proc; src++) {
            int chunk_beginning = get_chunk_beginning(src, total_num_proc);
            int chunk_end = get_chunk_end(src, total_num_proc);
            for(int y = chunk_beginning; y < chunk_end; y++) {
                MPI_Recv(&(*map)[y][0], width, MPI_UNSIGNED_SHORT, src, TAG,
                         MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }
    } else {
        // Send chunk to process 0
        int chunk_beginning = get_chunk_beginning(my_rank, total_num_proc);
        int chunk_end = get_chunk_end(my_rank, total_num_proc);
        int dest = 0;
        for(int y = chunk_beginning; y < chunk_end; y++) {
            // 1 to skip halo
            MPI_Send(&(*map)[y][0], width, MPI_UNSIGNED_SHORT, dest, TAG,
                     MPI_COMM_WORLD);
        }
    }

    // Remove halo lines from map
    // (*map).erase((*map).begin() + 0);
    // (*map).pop_back();
}

void MPI_GameOfLife::makeNGeneration(int argc, char **argv, int num_of_generations) {
    int my_rank, total_num_proc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_num_proc);

    double start_time = MPI_Wtime();
    liveNGeneration(my_rank, total_num_proc, num_of_generations);
    double end_time = MPI_Wtime();

    if (my_rank == 0)
        std::cout << "Spend " << (end_time - start_time) * 1000 << " ms" << std::endl;

    MPI_Finalize();
}

void MPI_GameOfLife::randomFillMap() {
    // TODO
    // unsigned int x, y, seed;
    // int half_len;

	// // Get seed; random if 0
	// seed = (unsigned)time(NULL);

	// // Randomly initialise cell map with ~50% on pixels
	// std::cout << "Initializing" << std::endl;

	// srand(seed);

	// half_len = width * height;
    // while (half_len > 0)
    // {
    //     x = rand() % (width - 1);
	// 	y = rand() % (height - 1);

    //     if (((*map)[y][x] & 0x01) == 0) {
    //         setCell(x, y);
    //     }
        
    //     half_len--;
    // }
}

void inline MPI_GameOfLife::setCell(unsigned int x, unsigned int y) {
    (*map)[y][x] = 1;
}

void inline MPI_GameOfLife::clearCell(unsigned int x, unsigned int y) {
    (*map)[y][x] = 0;
}

unsigned int MPI_GameOfLife::getIteration() const {
    return (this->iteration);
}

std::string MPI_GameOfLife::getDump() const {
    std::stringstream ss;

    for (int y = 1; y < height + 1; y++) {
        for (int x = 0; x < width; x++) {
            if ((*map)[y][x] & 0x01)
                ss << "*";
            else
                ss << ".";
        }
        ss << std::endl;
    }

    return ss.str();
}
