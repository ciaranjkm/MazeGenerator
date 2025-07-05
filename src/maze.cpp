#include "include/maze.h"

maze::maze(const int& mazeWidth, const int& mazeHeight) {
	//set the width and height of the maze
	maze_width = mazeWidth;
	maze_height = mazeHeight;

	//set the selected cell to nullptr
	selected_cell = nullptr;
}

maze::~maze() {
	//delete selected_cell ptr
	//vectors are auto cleaned up
	selected_cell = nullptr;
}

//create a new empty maze 
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

//solve the maze using recursive backtracking algorithm
void maze::solve_maze_rb() {
	//generate a random index to start
	srand((unsigned)time(NULL));
	int random_index = rand() % get_total_maze_size();

	//set the selected cell
	selected_cell = get_cell(random_index);
	selected_cell->visited = true;

	//while we haven't visited all cells
	while (visited_cells.size() < get_total_maze_size()) {
		//select a random direction to move
		srand((unsigned)time(NULL));
		int random_direction = rand() % 4; // 0: up, 1: right, 2: down, 3: left
		maze_cell* target_cell = nullptr;

		switch (random_direction) {
			case 0: // up
				if (selected_cell->y < 1) { continue; }
				target_cell = get_cell(selected_cell->cell_id - maze_width);
				target_cell->visited = true;

				break;

			case 1: //right
				if (selected_cell->x > maze_width - 2) { continue; } 
				target_cell = get_cell(selected_cell->cell_id + 1);
				target_cell->visited = true;

				break;

			case 2: //down
				if (selected_cell->y > maze_height - 2) { continue; }
				target_cell = get_cell(selected_cell->cell_id + maze_width);
				target_cell->visited = true;

				break;

			case 3: //left
				if (selected_cell->x < 1) { continue; }
				target_cell = get_cell(selected_cell->cell_id - 1);
				target_cell->visited = true;

				break;

			default:
				continue; //invalid dir
		}

		if (target_cell != nullptr) {
			return;
		}
	}
}

//return a ptr to a cell at specified index
maze_cell* maze::get_cell(const int& index) {
	//return the vector of maze cells
	return &original_maze_cells[index];
}

//get the total size of the maze 
int maze::get_total_maze_size()  {
	//return the width of the maze
	return maze_width * maze_height;
}

//get the width of the maze
int maze::get_maze_width() {
	//return the width of the maze
	return maze_width;
}

//get the height of the maze
int maze::get_maze_height() {
	//return the height of the maze
	return maze_height;
}