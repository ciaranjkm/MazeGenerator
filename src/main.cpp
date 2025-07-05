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
					//create a new maze object
					m = new maze(10, 10);

					//generate a new empty maze
					m->generate_new_empty_maze();

					//draw the empty maze on screen 
					draw_maze(renderer, m);

					break;

				//on key s solve the maze
				case SDLK_S:
					m->solve_maze_rb();

					draw_maze(renderer, m);

					break;
				}

				}

		}

		SDL_Delay(1); //limit to let cpu sleep a bit
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
