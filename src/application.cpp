#include "include/application.h"
#include "include/maze.h"

application::application() {
	window = nullptr;
	renderer = nullptr;

	active_maze = nullptr;
}

application::~application() {
	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	SDL_Quit();

	if (active_maze != nullptr) {
		delete active_maze;
		active_maze = nullptr;
	}
}

void application::maze_thread_function(SDL_Renderer* renderer) {
	if (active_maze != nullptr) {
		active_maze->solve_maze_rb(renderer);
	}
}

void application::run() {
	//sdl startup
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialise, error: " << SDL_GetError() << "\n";
		return;
	}

	//create sdl window
	SDL_Window* window = SDL_CreateWindow(
		"Mystz' Maze Generator",
		620, 620,
		0);

	if (window == nullptr) {
		std::cout << "SDL could not create a window, error: " << SDL_GetError() << "\n";
		return;
	}

	//create sdl renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		0
	);

	if (renderer == nullptr) {
		std::cout << "SDL could not create a renderer, error: " << SDL_GetError() << "\n";
		return;
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
				if (active_maze != nullptr) {
					active_maze->stop_thread = true;
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
					if (!(active_maze != nullptr)) {
						//create a new maze object
						active_maze = new maze(25, 25);

						//generate a new empty maze
						active_maze->generate_new_empty_maze();

						//draw the empty maze on screen 
						active_maze->draw_maze(renderer, false);

						//start a new thread that solves the maze
						maze_thread = std::thread(&application::maze_thread_function, this, renderer);
					}
					else {
						std::cout << "ALREADY HAVE A MAZE. PRESS D TO DELETE CURRENT MAZE\n";
					}
					break;
					//on key d delete current maze if it exists
				case SDLK_D:
					if (active_maze != nullptr) {
						active_maze->stop_thread = true;
						if (maze_thread.joinable()) {
							maze_thread.join();
						}

						SDL_SetRenderDrawColor(renderer, 248, 248, 255, 255);
						SDL_RenderClear(renderer); //clear the renderer
						SDL_RenderPresent(renderer); //present the cleared renderer

						delete active_maze;
						active_maze = nullptr;

						std::cout << "FREED MAZE OBJECT FROM MEMORY. CAN NOW CREATE A NEW MAZE.\n";
					}
					else {
						std::cout << "CANNOT DELETE MAZE, THERE IS NO MAZE TO DELETE. CREATE ONE BY PRESSING M\n";
					}
					break;

				//on key p, take a screenshot of the screen at the time
				case SDLK_P:
					if(active_maze != nullptr) {
						active_maze->allow_generation = false;
						active_maze->draw_maze(renderer, true); //redraw maze with screenshot flag set
						active_maze->allow_generation = true;
					}
					else {
						std::cout << "CANNOT SCREENSHOT MAZE THERE IS NOT ONE IN MEMORY.";
					}
					break;
				}
			}
		}

		//if there is a maze being generated or solved
		if (active_maze != nullptr) {
			//is the maze requesting a draw?
			if (active_maze->draw_requested) {
				active_maze->draw_maze(renderer, false); //draw the maze from the main thread
				active_maze->draw_requested = false;
				active_maze->allow_generation = true;
			}

			//is the maze solved? should we stop the thread?
			if (active_maze->solved) {
				if (maze_thread.joinable()) {
					active_maze->stop_thread = true; //stop the maze thread if it is still running
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
	if (active_maze != nullptr) {
		delete active_maze;
		active_maze = nullptr;
	}
}