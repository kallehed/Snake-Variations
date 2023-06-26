
#pragma once
#include "very_general.h"
#include "player_related.h"

#define EVIL_SNAKE_MAX_LENGTH 10
typedef struct 
{
    Int length;
    Dir direction;
    Pos positions[EVIL_SNAKE_MAX_LENGTH];
} Seeker_Snake;


// #define GAME_STATE1_TOTAL_EVIL_SNAKES 10
typedef struct 
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
    Int evil_snake_index;
    Seeker_Snake seeker;
} Game_State_Seeker;

void game_state_init_Seeker(Game_State_Seeker *new_g);
void evil_snake_move(Evil_Snake *snake, World_State0 *w);
void evil_snake_draw(const Evil_Snake *snake, const World_State0 *w);
// returns true on collision
bool evil_snake_player_collision_logic(const Evil_Snake *snake, const Player *player);

Level_Return game_state_frame_Seeker(Game_State_Seeker *g);
