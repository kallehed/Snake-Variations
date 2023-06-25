#pragma once
#include "very_general.h"
#include "player_related.h"
#include "box_general.h"

#define GAME_STATE4_BOXES 20
typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    Box boxes[GAME_STATE4_BOXES];
    double time_for_move;
} Game_State4;

void game_state4_init(Game_State4 *new_g);

Level_Return game_state4_frame0(Game_State4 *g);
