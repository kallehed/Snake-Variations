#pragma once
#include "very_general.h"
#include "player_related.h"

typedef struct {
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;

} Game_State_StaticPlatformer;

void game_state_init_StaticPlatformer(Game_State_StaticPlatformer *new_g); 

Level_Return game_state_frame_StaticPlatformer(Game_State_StaticPlatformer *g);
