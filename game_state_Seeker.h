
#pragma once
#include "very_general.h"
#include "player_related.h"

#define SEEKER_MAX_LENGTH 10
typedef struct 
{
    Int length;
    Dir direction;
    Pos positions[SEEKER_MAX_LENGTH];
} Seeker_Snake;


// #define GAME_STATE1_TOTAL_EVIL_SNAKES 10
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

void game_state_init_Seeker(Game_State_Seeker *new_g);
// void seeker_move(Seeker_Snake *snake, World_State0 *w);
// void seeker_draw(const Seeker_Snake  *snake, const World_State0 *w);
// returns true on collision
// bool seeker_player_collision_logic(const Seeker_Snake  *snake, const Player *player);

Level_Return game_state_frame_Seeker(Game_State_Seeker *g);
