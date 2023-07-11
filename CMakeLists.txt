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

# Collect all source files from the source directory 
# WHEN ADDING NEW FILES DON't FORGET TO RE-SAVE THIS FILE
aux_source_directory(${SOURCE_DIR} SOURCE_FILES)

# Add your target executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4) 
else()
	IF(CMAKE_BUILD_TYPE MATCHES Debug)
		message("KALLEHEDS APPLICATION IS IN DEBUG")
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -fsanitize=address
			-Wcast-align -Wunused -Wshadow -Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wno-missing-braces -Wimplicit-fallthrough -Wcast-qual -Wwrite-strings -Wbad-function-cast -Wfloat-equal -Wformat=2 -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnested-externs -Wredundant-decls -Wsequence-point -Wstrict-prototypes -Wswitch -Wundef -Wunreachable-code -Wunused-but-set-parameter)
		target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
	else()
		message("KALLEHEDS IS RELEASE")
	endif()
endif()

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
