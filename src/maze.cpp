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
    std::cout << "New empty maze generated with width: " << maze_width << " and height: " << maze_height << std::endl;
}

void maze::solve_maze_rb(SDL_Renderer* renderer) {
	//generate a random index to start
	srand((unsigned)time(NULL));
	int random_index = rand() % get_total_maze_size();

	//set the selected cell
	selected_cell = get_cell(random_index);
	selected_cell->visited = true;
	visited_cells.push_back(selected_cell); //add the first cell as visited

	//while we haven't visited all cells
	while (visited_cells.size() < get_total_maze_size()) {
		//select a random direction to move
		srand((unsigned)time(NULL));
		int random_direction = get_random_int(0, 3); // 0: up, 1: right, 2: down, 3: left
		maze_cell* target_cell = nullptr;

		switch (random_direction) {
			case 0:
				//check if we can move up?
				if (selected_cell->y < 1) { 
					continue; 
				}

				//get the target cell above selected cell
				target_cell = get_cell(selected_cell->cell_id - maze_width);

				//remove the walls between the selected cell and target cell
				target_cell->bottom = false; //remove bottom of target cell
				selected_cell->top = false; //remove top of selected cell

				break;

			case 1:
				//check if we can move right?
				if (selected_cell->x > maze_width - 2) { 
					continue; 
				} 

				target_cell = get_cell(selected_cell->cell_id + 1);

				target_cell->left = false; 
				selected_cell->right = false; 

				break;

			case 2: 
				//check if we can move down?
				if (selected_cell->y > maze_height - 2) { 
					continue; 
				}

				target_cell = get_cell(selected_cell->cell_id + maze_width);

				target_cell->top = false;
				selected_cell->bottom = false;

				break;

			case 3: 
				//check if we can move left?
				if (selected_cell->x < 1) { 
					continue;
				}
				target_cell = get_cell(selected_cell->cell_id - 1);

				target_cell->right = false;
				selected_cell->left = false;

				break;

			default:
				continue; //invalid dir
		}

		if (target_cell == nullptr) {
			continue; //no target cell found
		}

		//check if the target cell has already been visited?
		if (target_cell->visited) {
			continue;
		}

		//if the cell has not been visited already all it to the visited cells vector and update the selected cell

		target_cell->visited = true; //mark the cell as visited
		visited_cells.push_back(target_cell); //add to visited cells

		selected_cell = target_cell; // update the selected cell

		draw_maze(renderer, this);
		SDL_Delay(16);
	}
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

void maze::draw_maze(SDL_Renderer* renderer, maze* maze) {
	//draw the background in off-white 
	SDL_Color* maze_background_colour = new SDL_Color{ 248, 248, 255, 255 };
	clear_background(renderer, *maze_background_colour);
	delete maze_background_colour;

	//get the height and width of the maze to draw each cell correctly
	int maze_width = maze->get_maze_width();
	int maze_height = maze->get_maze_height();

	//draw each cell in the maze
	for (int i = 0; i < maze->get_total_maze_size(); i++) {
		draw_maze_cell(renderer, maze->get_cell(i), maze_width, maze_height);
	}

	SDL_RenderPresent(renderer);
}

//PRIVATE FUNCTIONS

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