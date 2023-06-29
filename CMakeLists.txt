cmake_minimum_required(VERSION 3.15)
project(my_project)


set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(raylib)

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 --shell-file ../shell.html ")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
endif ()

add_executable(${PROJECT_NAME} main.c level_declarations.h box_general.h box_general.c game_cutscenes.h game_cutscenes.c game_state0.h game_state0.c game_state1.h game_state1.c game_state2.h game_state2.c game_state3.h game_state3.c game_state4.h game_state4.c game_state_maze.h game_state_maze.c game_state_YouFood.h game_state_YouFood.c player_related.h player_related.c very_general.h very_general.c game_state_StaticPlatformer.h game_state_StaticPlatformer.c game_state_Seeker.h game_state_Seeker.c game_state_OpenWorld.h game_state_OpenWorld.c maze_stuff.h maze_stuff.c snake_pather.h snake_pather.c)

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
