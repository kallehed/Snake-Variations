#pragma once
#include "very_general.h"
#include "player_related.h"
#include "box_general.h"

#define GAME_STATE2_BOXES 2
typedef struct
{
    World_State0 w;
    Player player;
    Box boxes[GAME_STATE2_BOXES];
    double time_for_move;
} Game_State2;

void game_state2_init(Game_State2 *new_g);

Level_Return game_state2_frame0(Game_State2 *g);
