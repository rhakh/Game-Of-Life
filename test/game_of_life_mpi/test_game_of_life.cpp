#include <iostream>
#include "catch.hpp"
#include "./game_of_life.hpp"
#include "test_helpers.hpp"

Grid grid_1 = grid_int_to_grid({
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 }
    });

Grid grid_2 = grid_int_to_grid({
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0 }
    });

Grid conways_pentadecathlon_state_11 = grid_int_to_grid({
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            });

Grid conways_pentadecathlon_state_7 = grid_int_to_grid({
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            });

void print_grid(Grid &grid, std::string name) {
    std::cout << "--- Print " << name << " grid:" << std::endl;
    for (int y = 0; y < grid[0].size(); y++) {
        for (int x = 0; x < grid.size(); x++) {
            std::cout << (int)(grid[x][y] & 0x01) << " ";
        }
        std::cout << std::endl;
    }
}

void clear_num_of_neighbours(Grid &grid) {
    for (int y = 0; y < grid[0].size(); y++) {
        for (int x = 0; x < grid.size(); x++) {
            grid[x][y] &= 0x01;
        }
    }
}

TEST_CASE("23/3 dead with 3 neighbors → born")
{
    int alive_neighbors = 3;
    REQUIRE( next_cell_state(dead, alive_neighbors) == alive );
}

TEST_CASE("23/3 dead with 2 neighbors → dead")
{
    int alive_neighbors = 2;
    REQUIRE( next_cell_state(dead, alive_neighbors) == dead );
}

TEST_CASE("23/3 alive with 2 neighbors → alive")
{
    int alive_neighbors = 2;
    REQUIRE( next_cell_state(alive, alive_neighbors) == alive );
}

TEST_CASE("23/3 alive with 3 neighbors → alive")
{
    int alive_neighbors = 3;
    REQUIRE( next_cell_state(alive, alive_neighbors) == alive );
}

TEST_CASE("23/3 alive with 4 neighbors → dead")
{
    int alive_neighbors = 4;
    REQUIRE( next_cell_state(alive, alive_neighbors) == dead );
}

TEST_CASE("transpose_grid_int")
{
    vector<vector<int>> grid_int {
        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 1, 0, 0 }
    };
    vector<vector<int>> expected_transposed_grid {
        { 1, 1, 1, 1},
        { 0, 0, 1, 0},
        { 0, 0, 0, 0}
    };
    auto transposed_grid_int = transpose_grid_int(grid_int);
    REQUIRE( transposed_grid_int == expected_transposed_grid );
}

TEST_CASE("count_neighbors: not at edge of grid")
{
    REQUIRE( count_neighbors(2, 2, grid_1) == 2 );
    REQUIRE( count_neighbors(1, 1, grid_1) == 2 );
    REQUIRE( count_neighbors(2, 1, grid_1) == 3 );
    REQUIRE( count_neighbors(3, 2, grid_1) == 1 );
}

TEST_CASE("add_halo_to_grid")
{
    Grid grid = grid_int_to_grid({
            { 1, 1 },
            { 1, 1 },
                });
    Grid grid_with_halo = grid_int_to_grid({
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
                });
    REQUIRE( add_halo_to_grid(grid) == grid_with_halo );
}

TEST_CASE("remove_halo_from_grid")
{
    Grid grid_with_halo = grid_int_to_grid({
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
                });
    Grid grid_without_halo = grid_int_to_grid({
            { 1, 1 },
            { 1, 1 },
                });
    REQUIRE( remove_halo_from_grid(grid_with_halo) == grid_without_halo );
}

TEST_CASE("23/3 blinker one generation")
{
    Grid result = tick_grid_n_times(grid_1, 1);
    Grid expected = grid_2;

    // print_grid(grid_1, "grid_1");
    // print_grid(result, "result");
    // print_grid(expected, "expected");
    clear_num_of_neighbours(result);

    REQUIRE( result == expected );
}

TEST_CASE("23/3 blinker two and four generations should be the same")
{
    // Grid result = tick_grid_n_times(tick_grid_n_times(grid_1, 1), 1);
    Grid result = tick_grid_n_times(grid_1, 2);
    Grid expected = grid_1;

    // print_grid(grid_1, "grid_1");
    // print_grid(result, "result");
    // print_grid(expected, "expected");
    clear_num_of_neighbours(result);

    REQUIRE( result == expected );

    result = tick_grid_n_times(grid_1, 4);
    expected = grid_1;
    clear_num_of_neighbours(result);
    REQUIRE( result == expected );
}

TEST_CASE("23/3 blinker conway's pentadecathlon")
{
    Grid result = tick_grid_n_times(conways_pentadecathlon_state_11, 11);
    Grid expected = conways_pentadecathlon_state_7;

    // print_grid(conways_pentadecathlon_state_11, "conways_pentadecathlon_state_11");
    // print_grid(result, "result");
    // print_grid(expected, "expected");
    clear_num_of_neighbours(result);

    REQUIRE( result == expected );

    // 15 additional cycles shouldn't change anything as the period is 15
    result = tick_grid_n_times(conways_pentadecathlon_state_11, 26);

    // print_grid(conways_pentadecathlon_state_11, "conways_pentadecathlon_state_11 26");
    // print_grid(result, "result");
    // print_grid(expected, "expected");
    clear_num_of_neighbours(result);

    REQUIRE( result == expected );
}


TEST_CASE("23/3 huge grid to create a case where a tick is slow")
{
    int NUMBER_OF_TIMES_TO_ADD_HALO = 30;
    Grid huge_conways_pentadecathlon_state_11 = add_halo_n_times(
        conways_pentadecathlon_state_11,
        NUMBER_OF_TIMES_TO_ADD_HALO);
    Grid huge_conways_pentadecathlon_state_7 = add_halo_n_times(
        conways_pentadecathlon_state_7,
        NUMBER_OF_TIMES_TO_ADD_HALO);
    Grid result = tick_grid_n_times(huge_conways_pentadecathlon_state_11, 11);
    clear_num_of_neighbours(result);
    REQUIRE( result == huge_conways_pentadecathlon_state_7 );
}

TEST_CASE("calculate_chunk_beginning")
{
    // params: grid_size, chunk_number/process_rank, number_of_chunks/processes
    REQUIRE( calculate_chunk_beginning(8, 0, 1) == 0 );
    REQUIRE( calculate_chunk_beginning(7, 0, 1) == 0 );
    REQUIRE( calculate_chunk_beginning(5, 0, 1) == 0 );

    REQUIRE( calculate_chunk_beginning(8, 0, 3) == 0 );
    REQUIRE( calculate_chunk_beginning(8, 1, 3) == 3 );
    REQUIRE( calculate_chunk_beginning(8, 2, 3) == 6 );

    REQUIRE( calculate_chunk_beginning(7, 0, 3) == 0 );
    REQUIRE( calculate_chunk_beginning(7, 1, 3) == 3 );
    REQUIRE( calculate_chunk_beginning(7, 2, 3) == 5 );

    REQUIRE( calculate_chunk_beginning(5, 0, 3) == 0 );
    REQUIRE( calculate_chunk_beginning(5, 1, 3) == 2 );
    REQUIRE( calculate_chunk_beginning(5, 2, 3) == 4 );

    REQUIRE( calculate_chunk_beginning(11, 0, 5) == 0 );
    REQUIRE( calculate_chunk_beginning(11, 1, 5) == 3 );
    REQUIRE( calculate_chunk_beginning(11, 2, 5) == 5 );
    REQUIRE( calculate_chunk_beginning(11, 3, 5) == 7 );
    REQUIRE( calculate_chunk_beginning(11, 4, 5) == 9 );
}

TEST_CASE("calculate_chunk_end")
{
    // params: grid_size, chunk_number/process_rank, number_of_chunks/processes
    REQUIRE( calculate_chunk_end(8, 0, 1) == 8 );
    REQUIRE( calculate_chunk_end(7, 0, 1) == 7 );
    REQUIRE( calculate_chunk_end(5, 0, 1) == 5 );

    REQUIRE( calculate_chunk_end(8, 0, 3) == 3 );
    REQUIRE( calculate_chunk_end(8, 1, 3) == 6 );
    REQUIRE( calculate_chunk_end(8, 2, 3) == 8 );

    REQUIRE( calculate_chunk_end(7, 0, 3) == 3 );
    REQUIRE( calculate_chunk_end(7, 1, 3) == 5 );
    REQUIRE( calculate_chunk_end(7, 2, 3) == 7 );

    REQUIRE( calculate_chunk_end(5, 0, 3) == 2 );
    REQUIRE( calculate_chunk_end(5, 1, 3) == 4 );
    REQUIRE( calculate_chunk_end(5, 2, 3) == 5 );

    REQUIRE( calculate_chunk_end(11, 0, 5) == 3 );
    REQUIRE( calculate_chunk_end(11, 1, 5) == 5 );
    REQUIRE( calculate_chunk_end(11, 2, 5) == 7 );
    REQUIRE( calculate_chunk_end(11, 3, 5) == 9 );
    REQUIRE( calculate_chunk_end(11, 4, 5) == 11 );
}
