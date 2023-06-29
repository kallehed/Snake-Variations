#pragma once

#include "very_general.h"
#include "player_related.h"
#include <inttypes.h>

#define Maze0_Cell_Empty 0
#define Maze0_Cell_Wall 1

typedef uint8_t Maze0_Cell;

// draws in normal colors
void maze0_draw(const Maze0_Cell maze[], const Int maze_width, const Int maze_height, const World_State0 *w);
// - is empty, | is wall, F is placing food, S is signifier but does not do anything
void maze0_init_from_string(const char **const map, const Int width, const Int height, Food foods[], Maze0_Cell maze[]);
