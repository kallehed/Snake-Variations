#pragma once

#include "maze_stuff.h"
#include "player_related.h"
#include "very_general.h"

typedef struct
{
    Coord from_x;
    float speed;
} Speed_Point;

#define GS_OBSCOURSE_MAX_SPEED_POINTS 10
#define GS_OBSCOURSE_WIDTH 200
#define GS_OBSCOURSE_HEIGHT 15
#define GS_OBSCOURSE_MAX_FOODS 4
#define GS_OBSCOURSE_PLAYER_START_LENGTH 2
typedef struct
{
    World_State0 w;
    Player player;
    Food foods[GS_OBSCOURSE_MAX_FOODS];
    double time_for_move;
    float cam_x;
    Speed_Point speed_points[GS_OBSCOURSE_MAX_SPEED_POINTS];
    Maze0_Cell maze[GS_OBSCOURSE_HEIGHT][GS_OBSCOURSE_WIDTH];
} Game_State_ObsCourse;

void game_state_init_ObsCourse(Game_State_ObsCourse *new_g);
Level_Return game_state_frame_ObsCourse(Game_State_ObsCourse *g);
