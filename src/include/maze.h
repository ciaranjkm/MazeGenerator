#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <iostream>

typedef struct maze_cell {
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

	//drawing functions
	//clear the background of the screen to a specific colour
	void clear_background(SDL_Renderer* renderer, SDL_Color color);
	//draw a individual maze cell
	void draw_maze_cell(SDL_Renderer* renderer, maze_cell* maze_cell, const int& maze_width, const int& maze_height);
	//draw the entire maze 
	void draw_maze(SDL_Renderer* renderer, maze* maze);

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