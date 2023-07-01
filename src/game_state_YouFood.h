#pragma once
#include "very_general.h"
#include "player_related.h"

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    Dir food_dir;
    double time_for_move;
} Game_State_YouFood;

void game_state_YouFood_init(Game_State_YouFood *new_g);
Level_Return game_state_YouFood_frame(Game_State_YouFood *g);
