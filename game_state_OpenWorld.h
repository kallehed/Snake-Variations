#pragma once
#include "very_general.h"
#include "player_related.h"

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
} Game_State_OpenWorld;

void game_state_init_OpenWorld(Game_State_OpenWorld *new_g);

Level_Return game_state_frame_OpenWorld(Game_State_OpenWorld *g);
