# CMakeList.txt : CMake project for simpleMazeGame, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("simpleMazeGame")

include(FetchContent)
FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL
	GIT_TAG release-3.2.x
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    GIT_TAG release-3.2.x
)

set(SDLIMAGE_AVIF OFF)

FetchContent_MakeAvailable(SDL3_image)

# Add source to this project's executable.
add_executable (simpleMazeGame "src/main.cpp" "src/include/maze.h" "src/maze.cpp" "src/application.cpp" "src/include/application.h")

target_link_libraries(simpleMazeGame PUBLIC SDL3_image::SDL3_image SDL3::SDL3)

if(WIN32)
    add_custom_command(TARGET simpleMazeGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>  
            $<TARGET_FILE_DIR:simpleMazeGame>  
    )
    add_custom_command(TARGET simpleMazeGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_image::SDL3_image>  
            $<TARGET_FILE_DIR:simpleMazeGame>  
    )
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET simpleMazeGame PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
