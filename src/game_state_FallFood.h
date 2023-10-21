#pragma once
#include "maze_stuff.h"
#include "player_related.h"
#include "very_general.h"

#define GS_FALLFOOD_MAX_SPEED_POINTS 10
#define GS_FALLFOOD_WIDTH 20
#define GS_FALLFOOD_HEIGHT 1000
#define GS_FALLFOOD_PLAYER_START_LENGTH 2
#define GS_FALLFOOD_TOTAL_FOOD_TO_GET 10
typedef struct
{
    World_State0 w;
    Player *player;
    Food food;
    double time_for_move;
    float cam_y;
	float cam_y_speed;
    Maze0_Cell maze[GS_FALLFOOD_HEIGHT][GS_FALLFOOD_WIDTH];
} GS_FallFood;

void game_state_init_FallFood(GS_FallFood *new_g, Allo *allo);
Level_Return game_state_frame_FallFood(GS_FallFood *g);
