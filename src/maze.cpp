#include "include/maze.h"

maze::maze(const int& mazeWidth, const int& mazeHeight) {
	maze_width = mazeWidth;
	maze_height = mazeHeight;

	selected_cell = nullptr;
}

maze::~maze() {
	//delete selected_cell ptr
	//vectors are auto cleaned up
	if (selected_cell != nullptr) {
		delete selected_cell;
		selected_cell = nullptr;
	}
}

void maze::generate_new_empty_maze() {
	//clear the old maze cells to start fresh in the same vector
	original_maze_cells.clear();

	//reset cell id counter
	int cell_ids = 0;

	//generate new cells for the maze
	for(int y = 0; y < maze_height; y++) {
		for(int x = 0; x < maze_width; x++) {
			maze_cell n_cell = maze_cell(cell_ids, x, y, false, true, true, true, true);
			original_maze_cells.push_back(n_cell);
			
			cell_ids++;
		}
	}

	//inform user of success
    std::cout << "New empty maze generated with width: " << maze_width << " and height: " << maze_height << std::endl;
}

maze_cell* maze::get_cell(const int& index) {
	//return the vector of maze cells
	return &original_maze_cells[index];
}

int maze::get_total_maze_size()  {
	//return the width of the maze
	return maze_width * maze_height;
}

int maze::get_maze_width() {
	//return the width of the maze
	return maze_width;
}

int maze::get_maze_height() {
	//return the height of the maze
	return maze_height;
}