#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include<vector>
using namespace std;

#include "game_of_life.hpp"

Grid grid_int_to_grid(vector<vector<int>> grid_int);
template<typename T> vector<vector<T>> transpose_grid_int(vector<vector<T>> grid_int);
Grid add_halo_n_times(Grid grid, int n);

#endif
