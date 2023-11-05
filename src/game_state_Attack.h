#pragma once
#include "player_related.h"
#include "seeker_snake.h"
#include "very_general.h"

#define GS_MAX_SNAKES 40
#define GS_TIME_TO_WIN 60
#define GS_PLAYER_INV_TIME 1.5f
typedef struct
{
    World_State0 w;
    Player *player;
    float player_inv_timer;
    double time_for_move;
    double start_time;
	Int frames_gone_without_spawn;
    Int evil_snake_index;
    Seeker_Snake snakes[GS_MAX_SNAKES];
} Game_State_Attack;

void game_state_init_Attack(Game_State_Attack *new_g, Allo *allo, Sound sounds[]);
Level_Return game_state_frame_Attack(Game_State_Attack *g);
