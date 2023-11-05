#pragma once

#include "box_general.h"
#include "maze_stuff.h"
#include "player_related.h"
#include "snake_pather.h"
#include "very_general.h"

#define GS_ZELDA_WIDTH 100
#define GS_ZELDA_HEIGHT 75
#define GS_ZELDA_PATHERS 6
#define GS_ZELDA_ROOM_WIDTH 20
#define GS_ZELDA_ROOM_HEIGHT 15
#define GS_ZELDA_ROOM_CHANGE_TIME 1.f
#define GS_ZELDA_MAX_POINTS 22
#define GS_ZELDA_MAX_BOXES 12
#define GS_ZELDA_PLAYER_INV_TIME 1.5f
typedef struct
{
    World_State0 w;
    Player *player;
    float player_inv_timer;
    // What zelda room they are in, set to -1 in beginning to notify that rooms shouldn't be changed at start
    Int room_x, room_y;
    // For cutscene when scrolling to next room to interpolate
    Int prev_room_x, prev_room_y;
    // counts down to zero;
    float room_change_timer;
    double time_for_move;
    Box boxes[GS_ZELDA_MAX_BOXES];
    Snake_Pather pathers[GS_ZELDA_PATHERS];
    Maze0_Cell maze[GS_ZELDA_HEIGHT][GS_ZELDA_WIDTH];
} GS_Zelda;

void game_state_init_Zelda(GS_Zelda *new_g, Allo *allo, Sound sounds[]);

Level_Return game_state_frame_Zelda(GS_Zelda *g);
