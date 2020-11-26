#include <iostream>
#include "test_helpers.hpp"


/* Hardcoded matrices need to accessed with [y][x] instead of [x][y]
   Transposing them allows to fix that.
   TODO remove from here and manage to compile when using transpose_grid() from
   game_of_life.cpp
*/
template<typename T>
vector<vector<T>> transpose_grid_int(vector<vector<T>> grid_int)
{
    auto transposed = vector<vector<int>>(grid_int[0].size(),
                                          vector<T>(grid_int.size()));
    for (unsigned int y = 0; y < grid_int.size(); ++y) {
        for (unsigned int x = 0; x < grid_int[0].size(); ++x) {
            transposed[x][y] = grid_int[y][x];
        }
    }
    return transposed;
}

Grid grid_int_to_grid(vector<vector<int>> grid_int)
{
    grid_int = transpose_grid_int(grid_int);
    Grid grid = Grid();
    for (auto row : grid_int) {
        auto cells_row = vector<CellState>();
        for (int cell : row) {
            if(cell == 1) {
                cells_row.push_back(alive);
            } else {
                cells_row.push_back(dead);
            }
        }
        grid.push_back(cells_row);
    }
    return grid;
}

Grid add_halo_n_times(Grid huge_grid, int n)
{
    for (int i = 0; i<n; ++i) {
        huge_grid =
            add_halo_to_grid(huge_grid);
    }
    return huge_grid;
}
