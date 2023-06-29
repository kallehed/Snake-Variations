#pragma once

#include "player_related.h"
#include "snake_pather.h"
#include "very_general.h"
#include <inttypes.h>

#define Maze0_Cell_Empty 0
#define Maze0_Cell_Wall 1

typedef int8_t Maze0_Cell;

// - is empty, | is wall, F is placing food, S is signifier but does not do anything
void maze0_init_from_string(const char *const *const map, const Int width, const Int height, Food foods[],
                            Maze0_Cell maze[], Snake_Pather pathers[], const Int pather_lengths[],
                            const Dir pather_dirs[], const World_State0 *w);
// draws in normal colors
void maze0_draw(const Maze0_Cell maze[], const Int maze_width, const Int maze_height, const World_State0 *w);
// returns whether player died going into self or not
bool maze0_player_move(Maze0_Cell maze[], const Int maze_width, Player *player, World_State0 *w);
