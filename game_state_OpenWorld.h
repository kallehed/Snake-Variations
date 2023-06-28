#pragma once
#include "player_related.h"
#include "very_general.h"

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move; 
} Game_State_OpenWorld;


