#include<iostream>
#include <numeric> // std::accumulate
#include <algorithm> // std::min
#include <mpi.h>

#include "game_of_life.hpp"

int TAG = 0;  // for MPI communications

Grid tick_grid_n_times(Grid grid, int times_to_tick)
{
    int my_rank, total_num_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_num_proc);
    double start_time = MPI_Wtime();

    

    Grid grid_with_halo = add_halo_to_grid(grid);
    for (int i = 0; i < times_to_tick; ++i) {
        grid_with_halo = tick_chunk(grid_with_halo, my_rank, total_num_proc);
    }

    int grid_width = grid_with_halo.size() - 2;
    int column_height = grid_with_halo[0].size() - 2;
    if (my_rank != 0) {
        // send chunk to process 0
        int chunk_beginning = calculate_chunk_beginning(grid_width, my_rank, total_num_proc);
        int chunk_end = calculate_chunk_end(grid_width, my_rank, total_num_proc);
        int dest = 0;
        for(int x = chunk_beginning; x < chunk_end; ++x) {
            int x_in_halo = x + 1;
            // [1] to not send halo
            MPI_Send(&grid_with_halo[x_in_halo][1], column_height, MPI_INT, dest, TAG,
                     MPI_COMM_WORLD);
        }
        return grid; // phony return to avoid warning
    } else {
        // receive other chunks and merge with the first one
        for(int src = 1; src < total_num_proc; ++src) {
            int chunk_beginning = calculate_chunk_beginning(grid_width, src, total_num_proc);
            int chunk_end = calculate_chunk_end(grid_width, src, total_num_proc);
            for(int x = chunk_beginning; x < chunk_end; ++x) {
                int x_in_halo = x + 1;
                MPI_Recv(&grid_with_halo[x_in_halo][1], column_height, MPI_INT, src, TAG,
                         MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }
        grid = remove_halo_from_grid(grid_with_halo);
        double end_time = MPI_Wtime();
        cout << times_to_tick << " ticks took " << (end_time-start_time)*1000 << " ms" << endl;
        return grid;
    }
}

Grid tick_chunk(Grid grid_with_halo, int my_rank, int num_proc)
{
    Grid result = grid_with_halo;
    int grid_width = grid_with_halo.size() - 2;
    int chunk_beginning = calculate_chunk_beginning(grid_width, my_rank, num_proc);
    // end index is not included in chunk
    int chunk_end = calculate_chunk_end(grid_width, my_rank, num_proc);

    // we will iterate only in the zone inside the halo,
    // the actual grid, represented with Xs
    // 0 0 0 0
    // 0 X X 0
    // 0 X X 0
    // 0 0 0 0

    // +1 offset because chunk is calculated without the halo
    for (int x = chunk_beginning+1; x < chunk_end+1; ++x) {
        for (uint y = 1; y < grid_with_halo[0].size()-1; ++y) { // -1 offset to not touch halo
            int neighbors = count_neighbors(x, y, grid_with_halo);
            CellState new_state = next_cell_state(grid_with_halo[x][y], neighbors);
            result[x][y] = new_state;
        }
    }
    int column_height = result[0].size() - 2 /*halo*/;
    bool there_is_chunk_at_left = my_rank != 0;
    if(there_is_chunk_at_left) {
        // receive other side of the left border
        int src = my_rank - 1;  // next process
        int my_first_column = chunk_beginning + 1/*halo*/;
        int other_side_border = my_first_column - 1;
        // MPI_INT because CellState enum is in fact an int
        // [1] to not send halo
        MPI_Recv(&result[other_side_border][1], column_height, MPI_INT, src, TAG,
                 MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        //  send my side of the left border
        int dest = my_rank - 1;  // previous process
        MPI_Send(&result[my_first_column][1], column_height, MPI_INT, dest, TAG,
                 MPI_COMM_WORLD);
    }
    int last_process = num_proc - 1;
    bool there_is_chunk_at_right = my_rank != last_process;
    if(there_is_chunk_at_right) {
        // send my side of the right border
        int dest = my_rank + 1;  // next process
        int my_last_column = chunk_end - 1/*chunk_end is exculded from chunk*/ + 1/*halo*/;
        MPI_Send(&result[my_last_column][1], column_height, MPI_INT, dest, TAG,
                 MPI_COMM_WORLD);
        // receive other side of the right border
        int src = my_rank + 1;  // next process
        int other_side_border = my_last_column + 1;
        MPI_Recv(&result[other_side_border][1], column_height, MPI_INT, src, TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    return result;
}

int calculate_chunk_beginning(int grid_size, int my_rank, int num_proc)
{
    int chunk_size = grid_size / num_proc;
    int rest = grid_size % num_proc;
    int beginning = chunk_size * my_rank;
    if (rest != 0) {
        if (my_rank == 0) { return beginning; }
        if (my_rank >= rest) { return beginning + rest; }
        beginning += my_rank;
    }
    return beginning;
}

int calculate_chunk_end(int grid_size, int my_rank, int num_proc)
{
    int next_chunk_begining = calculate_chunk_beginning(grid_size, my_rank+1, num_proc);
    return min(grid_size, next_chunk_begining);
}

/* Surrounds the grid with dead cells effectively creating a halo. It allows to
   simply handle the edges and corners.
*/
Grid add_halo_to_grid(Grid grid)
{
    add_dead_cells_left_and_right(grid);
    add_dead_cells_rows_at_top_and_bottom(grid);
    return grid;
}

void add_dead_cells_left_and_right(Grid& grid)
{
    for (vector<CellState>& row : grid) {
        row.insert(row.begin(), dead);
        row.push_back(dead);
    }
}

void add_dead_cells_rows_at_top_and_bottom(Grid& grid)
{
    auto row_to_insert = vector<CellState>();
    for (ulong i = 0; i < grid[0].size(); ++i) {
        row_to_insert.push_back(dead);
    }
    grid.insert(grid.begin(), row_to_insert);
    grid.push_back(row_to_insert);
}

Grid remove_halo_from_grid(Grid grid)
{
    remove_first_and_last_elements(grid);
    for (vector<CellState>& row : grid) {
        remove_first_and_last_elements(row);
    }
    return grid;
}

template<typename T>
void remove_first_and_last_elements(vector<T>& vect)
{
    vect.pop_back();
    vect.erase(vect.begin());
}

int count_neighbors(int x, int y, Grid grid)
{
    auto neighbors_cells = vector<CellState>();
    // row above
    neighbors_cells.push_back(grid[x-1][y-1]);
    neighbors_cells.push_back(grid[x-0][y-1]);
    neighbors_cells.push_back(grid[x+1][y-1]);
    // same row
    neighbors_cells.push_back(grid[x-1][y-0]);
    neighbors_cells.push_back(grid[x+1][y-0]);
    // row below
    neighbors_cells.push_back(grid[x-1][y+1]);
    neighbors_cells.push_back(grid[x-0][y+1]);
    neighbors_cells.push_back(grid[x+1][y+1]);
    int alive_neighbors = accumulate(neighbors_cells.begin(),
                                     neighbors_cells.end(),
                                     0,
                                     add_alive_cells);
    return alive_neighbors;
}

int add_alive_cells(int accumulator, CellState next_cell)
{
    if (next_cell == alive) {
        return accumulator + 1;
    } else {
        return accumulator;
    }
}

CellState next_cell_state(CellState current_state, int alive_neighbors)
{
    if(alive_neighbors == 3) return alive;
    if(current_state == alive) {
        if(alive_neighbors == 3 || alive_neighbors == 2) return alive;
    }
    return dead;
}

void show_grid(Grid grid)
{
    grid = transpose_grid(grid);
    for (auto row : grid) {
        for (CellState cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
}

/* Hardcoded matrices need to accessed with [y][x] instead of [x][y]
   Transposing them allows to fix that.
*/
template<typename T>
vector<vector<T>> transpose_grid(vector<vector<T>> grid)
{
    auto transposed = vector<vector<T>>(grid[0].size(),
                                        vector<T>(grid.size()));
    for (unsigned int y = 0; y < grid.size(); ++y) {
        for (unsigned int x = 0; x < grid[0].size(); ++x) {
            transposed[x][y] = grid[y][x];
        }
    }
    return transposed;
}
