#pragma once
#include "very_general.h"
#include "player_related.h"

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
} Game_State0;

void game_state0_init0(Game_State0 *new_g);
// Gigantic free fast
void game_state0_init1(Game_State0 *new_g);
void game_state0_init_GetSmall(Game_State0 *new_g);

// normal snake
Level_Return game_state0_frame0(Game_State0 *g);
// flashier snake
Level_Return game_state0_frame1(Game_State0 *g);
Level_Return game_state0_frameGetSmall(Game_State0 *g);
// gigantic free fast
Level_Return game_state0_frame2(Game_State0 *g);
//
Level_Return game_state0_frame_Spinny(Game_State0 *g);
