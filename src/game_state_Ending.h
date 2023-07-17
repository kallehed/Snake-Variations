#pragma once
#include "player_related.h"
#include "seeker_snake.h"
#include "very_general.h"

#define GS_ENDING_WIDTH 28
#define GS_ENDING_HEIGHT 21
#define GS_ENDING_MAX_SEEKERS 70 // 70 is good
typedef struct
{
    World_State0 w;
    double time_for_move;
    double seeker_time_for_moves[GS_ENDING_MAX_SEEKERS];
    double seeker_wait_times[GS_ENDING_MAX_SEEKERS];
    Seeker_Snake seekers[GS_ENDING_MAX_SEEKERS];
	double start_time;
	double offset_time;

	Int score;
	Int deaths;
	Int evilness;
} GS_Ending;

GS_Ending gs_init_Ending(Int score, Int deaths, Int evilness);
Level_Return gs_frame_Ending(GS_Ending *g);
