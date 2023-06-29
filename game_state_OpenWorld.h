#pragma once
#include "maze_stuff.h"
#include "player_related.h"
#include "very_general.h"

#define GAME_STATE_OPENWORLD_WIDTH 100
#define GAME_STATE_OPENWORLD_HEIGHT 100
#define GAME_STATE_OPENWORLD_FOODS 20
typedef struct
{
    World_State0 w;
    Player player;
    double time_for_move;
    Food foods[GAME_STATE_OPENWORLD_FOODS];
    Maze0_Cell maze[GAME_STATE_OPENWORLD_HEIGHT][GAME_STATE_OPENWORLD_WIDTH];
} Game_State_OpenWorld;

void game_state_init_OpenWorld(Game_State_OpenWorld *new_g);

Level_Return game_state_frame_OpenWorld(Game_State_OpenWorld *g);
