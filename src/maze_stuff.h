#pragma once

#include "player_related.h"
#include "snake_pather.h"
#include "very_general.h"
#include "box_general.h"
#include <inttypes.h>

#define Maze0_Cell_Empty 0
#define Maze0_Cell_Wall 1
#define Maze0_Cell_Button 2

typedef int8_t Maze0_Cell;

// - is empty, | is wall, F is placing food, S is signifier but does not do anything
void maze0_init_from_string(const char *const *const map, const Int width, const Int height, Food foods[],
                            Maze0_Cell maze[], Snake_Pather pathers[], const Int pather_lengths[],
                            const Dir pather_dirs[], Box boxes[], const World_State0 *w);
// draws in normal colors
void maze0_draw(const Maze0_Cell maze[], const Int maze_width, const Int maze_height, const World_State0 *w);
// returns whether player died going into self or not
bool maze0_player_move(const Maze0_Cell maze[], const Int maze_width, Player *player, const World_State0 *w);
// PURE, says whether the player can actually move
bool maze0_player_can_move(const Maze0_Cell maze[], const Int maze_width, const Player *const player,
                           const World_State0 *const w);

bool rect_intersect_maze0_wall(const Int r_x, const Int r_y, const Int r_width, const Int r_height,
                               const Maze0_Cell maze[], const Int maze_width);
