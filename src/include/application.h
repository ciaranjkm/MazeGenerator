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
	//CONSTRUCTOR AND DESTRUCTOR
	application();
	~application();

	//MAIN APPLICATION LOOP
	void run();

	//UI BUTTON FUNCTIONS
	void generate_new_maze_button_click(SDL_Renderer* renderer, const int& width, const int& height, const int& algorithm_to_use);
	void delete_current_maze_button_click(SDL_Renderer* renderer);
	void screenshot_maze_button_click(SDL_Renderer* renderer);

	//UI CONTAINERS (simplified run())
	void algorithm_selection_listbox(const char* algorithm_names[], int& algorithm_selected_index);
	void maze_settings_sliders(int& m_height, int& m_width, const int& min_width_height, const int& max_width_height);
	void generation_control_buttons(SDL_Renderer* renderer, const int& m_width, const int& m_height, const int& algorithm_selected_index);
	void extra_buttons(SDL_Renderer* renderer);

	//MULTITHREADING FUNCTIONS
	void maze_thread_function(const int& algorithm_to_use);

private:
	maze* active_maze;
	std::thread maze_thread;
};