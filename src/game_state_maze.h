#pragma once

#include "very_general.h"
#include "player_related.h"
#include "maze_stuff.h"
#include "snake_pather.h"

#define GAME_STATE_MAZE_FOODS 4
#define GAME_STATE_MAZE_WIDTH 40 // you CANT change these, hardcoded
#define GAME_STATE_MAZE_HEIGHT 30
#define GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS 2
typedef struct
{
    World_State0 w;
    Player *player;
    double time_for_move;
    Food foods[GAME_STATE_MAZE_FOODS];
    Snake_Pather evil_snake_paths[GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS];
    Maze0_Cell maze[GAME_STATE_MAZE_HEIGHT][GAME_STATE_MAZE_WIDTH];
} Game_State_Maze;

void game_state_Maze_init(Game_State_Maze *new_g, Allo *allo);
Level_Return game_state_Maze_frame(Game_State_Maze *g);

