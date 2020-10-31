#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <vector>
#include <stdint.h>
using namespace std;

// public
enum CellState { dead, alive };
// typedef uint8_t CellState;
// enum CellState_e { dead, alive };
typedef vector<vector<CellState>> Grid;
void show_grid(Grid grid);
template<typename T> vector<vector<T>> transpose_grid(vector<vector<T>> grid);
Grid tick_grid_n_times(Grid grid, int times_to_tick);


// private
Grid add_halo_to_grid(Grid grid);
Grid remove_halo_from_grid(Grid grid);
Grid tick_chunk(Grid grid_with_halo, int my_rank, int num_proc);
int calculate_chunk_beginning(int grid_size, int my_rank, int num_proc);
int calculate_chunk_end(int grid_size, int my_rank, int num_proc);
void add_dead_cells_left_and_right(Grid& grid);
void add_dead_cells_rows_at_top_and_bottom(Grid& grid);
template<typename T> void remove_first_and_last_elements(vector<T>& vect);
int count_neighbors(int x, int y, Grid grid);
int add_alive_cells(int accumulator, CellState next_cell);
CellState next_cell_state(CellState current_state, int alive_neighbors);

#endif
