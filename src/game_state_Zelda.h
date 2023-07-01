#pragma once
#include "maze_stuff.h"
#include "snake_pather.h"
#include "player_related.h"
#include "very_general.h"

#define GS_ZELDA_WIDTH 100
#define GS_ZELDA_HEIGHT 100
#define GS_ZELDA_FOODS 6
#define GS_ZELDA_PATHERS 10
#define GS_ZELDA_ROOM_WIDTH 20
#define GS_ZELDA_ROOM_HEIGHT 15
#define GS_ZELDA_ROOM_CHANGE_TIME 1.f
typedef struct
{
    World_State0 w;
    Player player;
	// What zelda room they are in, set to -1 in beginning to notify that rooms shouldn't be changed at start
	Int room_x, room_y; 
	// For cutscene when scrolling to next room to interpolate
	Int prev_room_x, prev_room_y;
	// counts down to zero;
	float room_change_timer;
    double time_for_move;
    Food foods[GS_ZELDA_FOODS];
	Snake_Pather pathers[GS_ZELDA_PATHERS];
    Maze0_Cell maze[GS_ZELDA_HEIGHT][GS_ZELDA_WIDTH];
} GS_Zelda;

void game_state_init_Zelda(GS_Zelda *new_g);

Level_Return game_state_frame_Zelda(GS_Zelda *g);
