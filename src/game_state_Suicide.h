#pragma once
#include "player_related.h"
#include "very_general.h"
#include "seeker_snake.h"

#define GAME_STATE_SUICIDE_FLASH_TIME (1.5f)
#define GAME_STATE_SUICIDE_WIDTH 28
#define GAME_STATE_SUICIDE_HEIGHT 21
#define GAME_STATE_SUICIDE_MAX_SEEKERS 4
typedef struct
{
    World_State0 w;
    Player *player;
    double time_for_move;
    float player_flash_timer; // starts at high number, reaches 0 -> stop flashing
    double seeker_time_for_moves[GAME_STATE_SUICIDE_MAX_SEEKERS];
    double seeker_wait_times[GAME_STATE_SUICIDE_MAX_SEEKERS];
    Seeker_Snake seekers[GAME_STATE_SUICIDE_MAX_SEEKERS];
} Game_State_Suicide;

void game_state_init_Suicide(Game_State_Suicide *new_g, Allo *allo);
Level_Return game_state_frame_Suicide(Game_State_Suicide *g);
