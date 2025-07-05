#include "include/common.h"
#include <SDL3/SDL.h>
#include "include/maze.h"
#include "include/maze_visuals.h"

int main(int argc, char** argv[])
{
	maze* m = nullptr;

	//sdl startup
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialise, error: " << SDL_GetError() << "\n";
		return -1;
	}

	//create sdl window
	SDL_Window* window = SDL_CreateWindow(
		"SDL3 Window",
		620, 620,
		0);

	if (window == nullptr) {
		std::cout << "SDL could not create a window, error: " << SDL_GetError() << "\n";
		return -2;
	}

	//create sdl renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		0
	);

	if (renderer == nullptr) {
		std::cout << "SDL could not create a renderer, error: " << SDL_GetError() << "\n";
		return -3;
	}

	//main application loop
	bool isRunning = true;
	while (isRunning) {

		//poll for events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			//on quit
			case SDL_EVENT_QUIT:
				isRunning = false;
				break;

			//on key down
			case SDL_EVENT_KEY_DOWN:
				switch (e.key.key)
				{
				//on key m generate a new maze (testing only, new maze started from cli)
				case SDLK_M:
					m = new maze(10, 10);
					m->generate_new_empty_maze();

					//draw the background in off-white 
					SDL_Color* maze_background_colour = new SDL_Color{ 248, 248, 255, 255 };
					clear_background(renderer, *maze_background_colour);
					delete maze_background_colour;
					
					//draw each cell of the maze
					for (int i = 0; i < m->get_total_maze_size(); i++) {
						draw_maze_cell(renderer, m->get_cell(i), m->get_maze_width(), m->get_maze_height());
					}

					//present the renderer to the window to show the maze
					SDL_RenderPresent(renderer);
				}

				break;
			}
		}

		SDL_Delay(1); //limit to rougly 60fps
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	if (m != nullptr) {
		delete m;
		m = nullptr;
	}

	return 0;
}
