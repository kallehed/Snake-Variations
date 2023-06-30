
#pragma once
#include "player_related.h"
#include "very_general.h"

#define SEEKER_MAX_LENGTH 10
typedef struct
{
    Int length;
    Dir direction;
    Pos positions[SEEKER_MAX_LENGTH];
} Seeker_Snake;

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
    double seeker_time_for_move;
    Seeker_Snake seeker;
    Int moves;
} Game_State_Seeker;

#define GAME_STATE_SUICIDE_FLASH_TIME (1.5f)
#define GAME_STATE_SUICIDE_WIDTH 28
#define GAME_STATE_SUICIDE_HEIGHT 21
typedef struct
{
    World_State0 w;
    Player player;
    double time_for_move;
    double seeker_time_for_move;
    float player_flash_timer; // starts at high number, reaches 0 -> stop flashing
    Seeker_Snake seeker;
} Game_State_Suicide;

void game_state_init_Seeker(Game_State_Seeker *new_g);
void game_state_init_Suicide(Game_State_Suicide *new_g);
// void seeker_move(Seeker_Snake *snake, World_State0 *w);
// void seeker_draw(const Seeker_Snake  *snake, const World_State0 *w);
// returns true on collision
// bool seeker_player_collision_logic(const Seeker_Snake  *snake, const Player *player);

Level_Return game_state_frame_Seeker(Game_State_Seeker *g);
Level_Return game_state_frame_Suicide(Game_State_Suicide *g);
