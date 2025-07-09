#pragma once

//include sdl for drawing the maze to the screen TODO:: ADD IMGUI SUPPORT FOR NON CLI OPERATION
#include <SDL3/SDL.h>
#include <vector>
#include <iostream>
#include <random>
#include <ctime>
#include <string>

typedef struct maze_cell {
	int cell_id;
	int x, y;
	bool visited;
	bool top, bottom, left, right;
}; 

class maze {
public:
	//CONSTRUCTOR AND DESTRUCTOR
	maze(const int& mazeWidth, const int& mazeHeight);
	~maze();

	//MAZE GENERATION FUNCTIONS
	void generate_new_empty_maze(); //fills original maze cells with new empty cells the size of the maze width * height
	void solve_maze_rb(); //solves the maze using reverse backtracking

	//DRAWING FUNCTIONS
	void clear_background(SDL_Renderer* renderer, SDL_Color color); //clear the background of the screen to a specific colour
	void draw_maze_cell(SDL_Renderer* renderer, maze_cell* maze_cell, const int& maze_width, const int& maze_height); //draw a individual maze cell
	void draw_maze(SDL_Renderer* renderer, const bool& screenshot, const int& animation_speed); //draw the entire maze 

	//control bools for multi threading drawing
	std::atomic_bool draw_requested = false;
	std::atomic_bool allow_generation = true;
	std::atomic_bool solved = false;
	std::atomic_bool stop_thread = false;

	//stores the original cells
	std::vector<maze_cell> original_maze_cells;


private:
	//MAZE GENERATION FUNCTIONS
	void destory_maze_walls(const int& direction, maze_cell* selected_cell, maze_cell* target_cell); //destory the wall between two cells

	//GETTERS FOR MAZE ATTRIBUTES
	maze_cell* get_cell(const int& index); //returns a cell at specific index in the maze

	int get_total_maze_size(); //returns maze dimentions
	int get_maze_width();
	int get_maze_height();

	//random function
	int get_random_int(const int& min, const int& max);


	//stores the visited cells, used to check for completeness of the maze
	std::vector<maze_cell*> visited_cells;

	//ptr to the currently selected cell
	maze_cell* selected_cell;

	//vars for maze sizing and any drawing stuff thats needed
	int maze_width;
	int maze_height;

};