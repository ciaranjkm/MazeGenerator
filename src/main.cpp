#include <SDL3/SDL.h>
#include "include/maze.h"
#include <thread>

void maze_thread_function(maze* m, SDL_Renderer* renderer) {
	m->solve_maze_rb(renderer);
}

int main(int argc, char** argv[])
{
	maze* m = nullptr;
	std::thread maze_thread;

	//sdl startup
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialise, error: " << SDL_GetError() << "\n";
		return -1;
	}

	//create sdl window
	SDL_Window* window = SDL_CreateWindow(
		"Mystz' Maze Generator",
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
				if (m != nullptr) {
					m->stop_thread = true;
				}
				if (maze_thread.joinable()) {
					//stop maze thread
					maze_thread.join();
				}
				break;

			//on key down
			case SDL_EVENT_KEY_DOWN:
				switch (e.key.key)
				{
				//on key m generate a new maze (testing only, new maze started from cli)
				case SDLK_M:
					if (!(m != nullptr)) {
						//create a new maze object
						m = new maze(25, 25);

						//generate a new empty maze
						m->generate_new_empty_maze();

						//draw the empty maze on screen 
						m->draw_maze(renderer, m);

						//create a thread to actually generate the maze (keep ui responsive)
						maze_thread = std::thread(maze_thread_function, m, renderer);
					}
					else {
						std::cout << "ALREADY HAVE A MAZE. PRESS D TO DELETE CURRENT MAZE\n";
					}
					break;
				//on key d delete current maze if it exists
				case SDLK_D:
					if (m != nullptr) {
						delete m;
						m = nullptr;
						std::cout << "FREED MAZE OBJECT FROM MEMORY. CAN NOW CREATE A NEW MAZE.";
					}
					else {
						std::cout << "CANNOT DELETE MAZE, THERE IS NO MAZE TO DELETE. CREATE ONE BY PRESSING M\n";
					}
					break;
				}
			}
		}

		if (m != nullptr) {
			if (m->draw_requested) {
				m->draw_maze(renderer, m); //draw the maze from the main thread
				m->draw_requested = false;
				m->allow_generation = true;
			}

			if (m->stop_thread) {
				if (maze_thread.joinable()) {
					maze_thread.join();
				}
			}
		}

		SDL_Delay(1); //limit to let cpu sleep a bit
	}

	//cleanup sdl at the end of the program
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	//free memory for maze object if it has been used
	if (m != nullptr) {
		delete m;
		m = nullptr;
	}

	return 0;
}