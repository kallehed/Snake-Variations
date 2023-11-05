#pragma once
#include "player_related.h"
#include "seeker_snake.h"
#include "very_general.h"

#define GS_YOUBLUE_TOTAL_SEEKERS 100
#define GS_YOUBLUE_TOTAL_PLAYERS 20
#define GS_YOUBLUE_PLAYER_INV_TIME 2.0f
typedef struct
{
    World_State0 w;
    double start_time;
    Player *players[GS_YOUBLUE_TOTAL_PLAYERS];
    float player_inv_timers[GS_YOUBLUE_TOTAL_PLAYERS];
    Int player_index;
    Dir turn_dir;
    double time_for_move;
    Int seeker_index;
    Seeker_Snake snakes[GS_YOUBLUE_TOTAL_SEEKERS];
} GS_YouBlue;

void gs_init_YouBlue(GS_YouBlue *new_g, Allo *allo, Sound sounds[]);
Level_Return gs_frame_YouBlue(GS_YouBlue *g);
