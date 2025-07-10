#include "include/application.h"
#include "include/maze.h"
#include <filesystem>

//CONSTRUCTOR AND DESTRUCTOR
application::application() {
	active_maze = nullptr;
}

application::~application() {
	if (active_maze != nullptr) {
		delete active_maze;
		active_maze = nullptr;
	}
}

//MAIN APPLICATION LOOP
void application::run() {
	//check if photos dir exists, if not create it so photos can be saved
	if (!std::filesystem::exists("photos")) {
		std::cout << "CREATING PHOTOS DIRECTORY\n";
		std::filesystem::create_directory("photos");
	}
	else {
		std::cout << "PHOTOS DIRECTORY FOUND\n";
	}

	//sdl startup
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL INITIALISATION HAS FAILED, ERROR: " << SDL_GetError() << "\n";
		return;
	}

	std::cout << "SDL INITIALISED\n";

	//create sdl window
	SDL_Window* window = SDL_CreateWindow(
		"Mystz' Maze Generator",
		620, 620,
		0);

	if (window == nullptr) {
		std::cerr << "SDL COULD NOT CREATE A WINDOW, ERROR: " << SDL_GetError() << "\n";
		return;
	}

	std::cout << "SDL WINDOW CREATED\n";

	//create sdl renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		0
	);

	if (renderer == nullptr) {
		std::cerr << "SDL COULD NOT CREATE A RENDERER, ERROR: " << SDL_GetError() << "\n";
		return;
	}

	std::cout << "SDL RENDERER CREATED\n====================\n";

	SDL_SetRenderDrawColor(renderer, 248, 248, 255, 255);
	SDL_RenderClear(renderer);

	//imgui setup

	//create imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // Get IO object for ImGui
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//setup style for imgui
	ImGui::StyleColorsDark();

	//setup backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	ImVec4 clear_colour = ImVec4(1, 1, 1, 0);

	//control variables for the imgui window
	int m_width = 10;
	int m_height = 10;
	const int max_width_height = 200;
	const int min_width_height = 1;

	int animation_speed = 16; //default speed in ms
	const int max_animation_speed = 250;
	const int min_animation_speed = 1;

	const char* algorithm_names[] = { "Recursive Backtracking", "W.I.P #1", "W.I.P #2" };
	int algorithm_selected_index = 0;

	//main application loop
	bool isRunning = true;
	while (isRunning) {
		//poll for events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			//process imgui events
			ImGui_ImplSDL3_ProcessEvent(&e); 

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

			case SDL_EVENT_KEY_DOWN:
				if (e.key.key == SDLK_ESCAPE) {
					SDL_Event* quit = new SDL_Event();
					quit->type = SDL_EVENT_QUIT;
					SDL_PushEvent(quit);

					delete quit;
					quit = nullptr;
				}
			}
		}

		//clear previous frame
		SDL_SetRenderDrawColor(renderer, 248, 248, 255, 255);
		SDL_RenderClear(renderer);

		//if there is a maze being generated or solved
		if (active_maze != nullptr) {
			active_maze->allow_generation = false;
			active_maze->draw_maze(renderer, false, animation_speed);
			active_maze->allow_generation = true;

			//is the maze solved? should we stop the thread?
			if (active_maze->solved) {
				if (maze_thread.joinable()) {
					active_maze->stop_thread = true; //stop the maze thread if it is still running
					maze_thread.join();
				}
			}
		}

		//start imgui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		//imgui window
		{
			ImGui::Begin("Maze Generator");

			algorithm_selection_listbox(algorithm_names, algorithm_selected_index);
			maze_settings_sliders(m_width, m_height, min_width_height, max_width_height);
			generation_control_buttons(renderer, m_width, m_height, algorithm_selected_index);
			extra_buttons(renderer);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
		
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

//UI BUTTON FUNCTIONS
void application::generate_new_maze_button_click(SDL_Renderer* renderer, const int& width, const int& height, const int& algorithm_to_use) {
	if (!(active_maze != nullptr)) {
		//create a new maze object
		active_maze = new maze(width, height);
		//generate a new empty maze
		active_maze->generate_new_empty_maze();
		//draw the empty maze on screen 
		active_maze->draw_maze(renderer, false, 1);
		//start a new thread that solves the maze
		maze_thread = std::thread(&application::maze_thread_function, this, algorithm_to_use);
	}
	else {
		std::cout << "ALREADY HAVE A MAZE. PRESS D TO DELETE CURRENT MAZE\n";
	}
}

void application::delete_current_maze_button_click(SDL_Renderer* renderer) {
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

		std::cout << "FREED MAZE OBJECT FROM MEMORY. CAN NOW CREATE A NEW MAZE.\n====================\n";
	}
	else {
		std::cout << "CANNOT DELETE MAZE, THERE IS NO MAZE TO DELETE. CREATE ONE BY PRESSING M\n";
	}
}

void application::screenshot_maze_button_click(SDL_Renderer* renderer) {
	if (active_maze != nullptr) {
		active_maze->allow_generation = false;
		active_maze->draw_maze(renderer, true, 1); //redraw maze with screenshot flag set
		active_maze->allow_generation = true;
	}
	else {
		std::cout << "CANNOT SCREENSHOT MAZE THERE IS NOT ONE IN MEMORY.\n";
	}
}

//UI CONTAINERS
void application::algorithm_selection_listbox(const char* algorithm_names[], int& algorithm_selected_index) {
	ImGui::SeparatorText("Maze Generation Algorithm");
	if (ImGui::BeginListBox("##algorithm_selection")) {
		for (int n = 0; n < IM_ARRAYSIZE(algorithm_names); n++) {
			const bool is_selected = (algorithm_selected_index == n);
			if (ImGui::Selectable(algorithm_names[n], is_selected)) {
				algorithm_selected_index = n;
			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndListBox();
	}
}

void application::maze_settings_sliders(int& m_height, int& m_width, const int& min_width_height, const int& max_width_height) {
	ImGui::SeparatorText("Maze Settings");
	ImGui::SliderInt("Width", &m_width, min_width_height, max_width_height);
	ImGui::SliderInt("Height", &m_height, min_width_height, max_width_height);
}

void application::generation_control_buttons(SDL_Renderer* renderer, const int& m_width, const int& m_height, const int& algorithm_selected_index) {
	ImGui::SeparatorText("Start/Stop Generation");
	if (ImGui::Button("Generate New Maze")) {
		generate_new_maze_button_click(renderer, m_width, m_height, algorithm_selected_index);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Current Maze")) {
		delete_current_maze_button_click(renderer);
	}
}

void application::extra_buttons(SDL_Renderer* renderer) {
	ImGui::SeparatorText("Extras");
	if (ImGui::Button("Screenshot")) {
		screenshot_maze_button_click(renderer);
	}
	ImGui::SameLine();
	if (ImGui::Button("Quit (ESC)")) {
		SDL_Event* quit = new SDL_Event();
		quit->type = SDL_EVENT_QUIT;
		SDL_PushEvent(quit);

		delete quit;
		quit = nullptr;
	}
}

//MULTITHREADING FUNCTIONS
void application::maze_thread_function(const int& algorithm_to_use) {
	if (active_maze != nullptr) {
		switch (algorithm_to_use) {
			case 0:
				active_maze->solve_maze_rb();
				break;

			default:
				active_maze->solve_maze_rb();
				break;
		}		
	}
}