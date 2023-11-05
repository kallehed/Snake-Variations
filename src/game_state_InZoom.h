
#pragma once
#include "very_general.h"
#include "player_related.h"

typedef struct
{
    World_State0 w;
    Player *player;
    Food food;
    double time_for_move;
} Game_State_InZoom;

void game_state_init_InZoom(Game_State_InZoom *new_g, Allo *allo, Sound sounds[]);
Level_Return game_state_frame_InZoom(Game_State_InZoom *g);
