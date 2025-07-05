#include <SDL3/SDL.h>
#include "maze.h"

void clear_background(SDL_Renderer* renderer, SDL_Color color) {
	//set the draw colour to specified colour
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	//clear the renderer with specified colour
	SDL_RenderClear(renderer);
}

void draw_maze_cell(SDL_Renderer* renderer, maze_cell* maze_cell, const int& maze_width, const int& maze_height) {
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
