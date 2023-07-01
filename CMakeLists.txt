cmake_minimum_required(VERSION 3.15)
project(my_project)


set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(raylib)

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 --shell-file ../shell.html ")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
endif ()

set(SOURCE_DIR src)

# Collect all source files from the source directory WHEN ADDING NEW FILES DON't FORGET TO cmake ..
aux_source_directory(${SOURCE_DIR} SOURCE_FILES)

# Add your target executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

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
