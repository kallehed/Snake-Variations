#pragma once
#include "player_related.h"
#include "seeker_snake.h"
#include "very_general.h"

#define GAME_STATE1_TOTAL_EVIL_SNAKES 10
typedef struct
{
    World_State0 w;
    Player *player;
    Food food;
    double time_for_move;
    Int evil_snake_index;
	Int evil_snake_hindrance;
    Seeker_Snake snakes[GAME_STATE1_TOTAL_EVIL_SNAKES];
} Game_State1;

typedef struct
{
    Game_State1 g1;
    double evil_snake_intervals[GAME_STATE1_TOTAL_EVIL_SNAKES];
    double evil_snake_time_for_moves[GAME_STATE1_TOTAL_EVIL_SNAKES];
} Game_State1_UnSync;

void game_state1_init(Game_State1 *new_g, Allo *allo);
void game_state1_init_UnSync(Game_State1_UnSync *new_g, Allo *allo);
Level_Return game_state1_frame0(Game_State1 *g);
Level_Return game_state1_frame_UnSync(Game_State1_UnSync *g);
