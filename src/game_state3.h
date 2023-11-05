#pragma once
#include "very_general.h"
#include "player_related.h"
// for EverGrowing
typedef struct
{
    World_State0 w;
    Player *player;
    Int player_points;
    Food food;
    double time_for_move;
} Game_State3;

void game_state3_init0(Game_State3 *new_g, Allo *allo, Sound sounds[]);

Level_Return game_state3_frame0(Game_State3 *g);
