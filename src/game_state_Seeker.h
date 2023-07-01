
#pragma once
#include "player_related.h"
#include "very_general.h"
#include "seeker_snake.h"

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
    double seeker_time_for_move;
    Seeker_Snake seeker;
    Int moves;
} Game_State_Seeker;

void game_state_init_Seeker(Game_State_Seeker *new_g);

Level_Return game_state_frame_Seeker(Game_State_Seeker *g);
