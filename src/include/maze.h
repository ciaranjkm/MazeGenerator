#pragma once

#include "common.h"
#include <vector>

struct maze_cell {
	int cell_id;
	int x, y;
	bool visited;
	bool top, bottom, left, right;
};

class maze {
public:
	maze(const int& mazeWidth, const int& mazeHeight);
	~maze();

	//fills original maze cells with new empty cells the size of the maze width * height
	void generate_new_empty_maze();

	//solves the maze using reverse backtracking
	void solve_maze_rb();

	//returns a cell at specific index in the maze
	maze_cell* get_cell(const int& index);

	//returns maze dimentions
	int get_total_maze_size();
	int get_maze_width();
	int get_maze_height();

private:
	//stores the original cells
	std::vector<maze_cell> original_maze_cells;

	//stores the visited cells, used to check for completeness of the maze
	std::vector<maze_cell> visited_cells;

	//ptr to the currently selected cell
	maze_cell* selected_cell;

	//vars for maze sizing and any drawing stuff thats needed
	int maze_width;
	int maze_height;
};