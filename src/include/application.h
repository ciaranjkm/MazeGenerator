#pragma once

#include "maze.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <thread>
#include <chrono>

class application {
public:
	application();
	~application();

	void run();
	void maze_thread_function(SDL_Renderer* renderer);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	maze* active_maze;
	std::thread maze_thread;
};