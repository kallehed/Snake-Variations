
#pragma once

#include "maze_stuff.h"
#include "player_related.h"
#include "snake_pather.h"
#include "very_general.h"

#define GS_FOODS 23
#define GS_WIDTH 47 // you CANT change these, hardcoded
#define GS_PIXEL_SIZE 18
#define GS_HEIGHT 35
#define GS_MAX_PATHERS 4
typedef struct
{
    World_State0 w;
    Player *player;
	Int points;
    double time_for_move;
    Food foods[GS_FOODS];
    Snake_Pather evil_snake_paths[GS_MAX_PATHERS];
    Maze0_Cell maze[GS_HEIGHT][GS_WIDTH];
} GS_OnceMaze;

void game_state_OnceMaze_init(GS_OnceMaze *new_g, Allo *allo, Sound sounds[]);
Level_Return game_state_OnceMaze_frame(GS_OnceMaze *g);
