#include "include/maze.h"

//CONSTRUCTORS AND DESTRUCTORS
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

//PUBLIC FUNCTIONS

//MAZE GENERATION FUNCTIONS
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
    std::cout << "NEW UNGENERATED MAZE CREATED WITH WIDTH: " << maze_width << " AND HEIGHT: " << maze_height << std::endl;
}

void maze::solve_maze_rb(SDL_Renderer* renderer){
	SDL_Delay(500);
	std::cout << "GENERATING MAZE OF TOTAL SIZE: " << get_total_maze_size() << " USING REVERSE BACKTRACKING ALGORITHM\n";
	
	//get random index to start maze from
	int random_index = get_random_int(0, original_maze_cells.size() - 1);

	//set the selected cell
	selected_cell = get_cell(random_index);
	selected_cell->visited = true;
	visited_cells.push_back(selected_cell); //add the first cell as visited

	//get a byte to store the directions checked already (bitwise or bottom nibble, 1 bit for each dir)
	unsigned char directions_checked = 0x00;

	//create an accumulator of the total visited cells start at 1 as have a starting cell
	int visited_cells_count = 1;

	//while we haven't visited all cells
	while (visited_cells_count < get_total_maze_size()) {
		if (stop_thread) {
			return;
		}

		if (allow_generation) {
			//select a random direction to move
			int random_direction = get_random_int(0, 3); // 0: up, 1: right, 2: down, 3: left
			maze_cell* target_cell = nullptr;

			//find a cell in the random direction
			switch (random_direction) {
			case 0:
				//check if we can move up?
				if (selected_cell->y < 1) {
					directions_checked |= (1 << random_direction);
					continue;
				}

				//get the target cell above selected cell
				target_cell = get_cell(selected_cell->cell_id - maze_width);

				break;

			case 1:
				//check if we can move right?
				if (selected_cell->x > maze_width - 2) {
					directions_checked |= (1 << random_direction);
					continue;
				}

				target_cell = get_cell(selected_cell->cell_id + 1);

				break;

			case 2:
				//check if we can move down?
				if (selected_cell->y > maze_height - 2) {
					directions_checked |= (1 << random_direction);
					continue;
				}

				target_cell = get_cell(selected_cell->cell_id + maze_width);
				break;

			case 3:
				//check if we can move left?
				if (selected_cell->x < 1) {
					directions_checked |= (1 << random_direction);
					continue;
				}
				target_cell = get_cell(selected_cell->cell_id - 1);
				break;

			default:
				continue; //invalid dir
			}

			//did we find a target cell
			if (target_cell == nullptr) {
				continue; //no target cell found
			}

			//if we have visited the target cell already, set random dir bit of directions tried
			if (target_cell->visited) {
				//add the direction to the checked directions
				directions_checked |= (1 << random_direction);

				//if we have checked all directions, step back in the maze and continue to the next cell
				if (directions_checked >= 0x0f) {
					//reset dir checked
					directions_checked = 0x00;
					//erase the target cell
					target_cell = nullptr;

					//if we have cells that can be back tracked to
					if (visited_cells.size() > 0) {
						//go back one cell 
						selected_cell = visited_cells[visited_cells.size() - 1];
						//pop that cell from the stack
						visited_cells.pop_back();
					}
				}
				//continue to the next cell
				continue;
			}

			if (stop_thread) {
				return;
			}

			//mark the cell as visited
			target_cell->visited = true;
			visited_cells.push_back(target_cell);

			//remove walls between cells
			destory_maze_walls(random_direction, selected_cell, target_cell);

			//update the selected cell to the target cell to move forward in the maze
			selected_cell = target_cell;

			//reset checked directions
			directions_checked = 0x00;

			//increment the visited cells count
			visited_cells_count++;

			//stop worker thread from generating more maze
			allow_generation = false;

			//allow main thread to render the maze
			draw_requested = true;
			SDL_Delay(16);
		}

		//allow delay to render on main thread if needed
		SDL_Delay(1);
	}

	std::cout << "MAZE COMPLETED.\n";
	stop_thread = true;
}

//DRAWING FUNCTIONS (SDL REQUIRED)
void maze::clear_background(SDL_Renderer* renderer, SDL_Color color) {
	//set the draw colour to specified colour
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	//clear the renderer with specified colour
	SDL_RenderClear(renderer);
}

void maze::draw_maze_cell(SDL_Renderer* renderer, maze_cell* maze_cell, const int& maze_width, const int& maze_height) {
	int cell_width = 600 / maze_width;
	int cell_height = 600 / maze_height;

	//convert grid coords to screen coords
	int drawX = maze_cell->x * cell_width;
	int drawY = maze_cell->y * cell_height;

	//add margin to drawx and drawy
	drawX += 10;
	drawY += 10;

	//draw the cell background if it has been visited
	if (maze_cell->visited) {
		SDL_SetRenderDrawColor(renderer, 128, 239, 128, 255);
		SDL_FRect* frect = new SDL_FRect();
		frect->x = drawX;
		frect->y = drawY;
		frect->w = cell_width;
		frect->h = cell_height;

		SDL_RenderFillRect(renderer, frect);
		delete frect;
	}

	//draw walls of the cell, if they exist
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	if (maze_cell->top) {
		SDL_RenderLine(renderer, drawX, drawY, drawX + cell_width, drawY);
	}
	if (maze_cell->right) {
		SDL_RenderLine(renderer, drawX + cell_width, drawY, drawX + cell_width, drawY + cell_height);
	}
	if (maze_cell->bottom) {
		SDL_RenderLine(renderer, drawX, drawY + cell_height, drawX + cell_width, drawY + cell_height);
	}
	if (maze_cell->left) {
		SDL_RenderLine(renderer, drawX, drawY, drawX, drawY + cell_height);
	}
}

void maze::draw_maze(SDL_Renderer* renderer) {
	//draw the background in off-white 
	SDL_Color* maze_background_colour = new SDL_Color{ 248, 248, 255, 255 };
	clear_background(renderer, *maze_background_colour);
	delete maze_background_colour;

	//get the height and width of the maze to draw each cell correctly
	int maze_width = get_maze_width();
	int maze_height = get_maze_height();

	//draw each cell in the maze
	for (int i = 0; i < get_total_maze_size(); i++) {
		draw_maze_cell(renderer, get_cell(i), maze_width, maze_height);
	}

	SDL_RenderPresent(renderer);
}

//PRIVATE FUNCTIONS

//MAZE GENERATION FUNCTIONS
void maze::destory_maze_walls(const int& direction, maze_cell* selected_cell, maze_cell* target_cell) {
	switch (direction) {
	case 0:
		selected_cell->top = false;
		target_cell->bottom = false;
		break;

	case 1: 
		selected_cell->right = false;
		target_cell->left = false;
		break;

	case 2:
		selected_cell->bottom = false;
		target_cell->top = false;
		break;

	case 3:
		selected_cell->left = false;
		target_cell->right = false;
		break;

	default:
		return;
	}
}

//random
int maze::get_random_int(const int& min, const int& max) {
	std::random_device rd;
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rd);
}

//GETTERS FOR MAZE ATTRIBUTES
maze_cell* maze::get_cell(const int& index) {
	//return the vector of maze cells
	return &original_maze_cells[index];
}

int maze::get_total_maze_size() {
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