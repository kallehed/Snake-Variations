
#pragma once
#include "very_general.h"
#include "player_related.h"

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
} Game_State_Accel;

void game_state_init_Accel(Game_State_Accel *new_g);

// normal snake
Level_Return game_state_frame_Accel(Game_State_Accel *g);
