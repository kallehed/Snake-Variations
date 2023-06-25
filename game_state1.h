#pragma once
#include "very_general.h"
#include "player_related.h"

#define EVIL_SNAKE_MAX_LENGTH 10
typedef struct Evil_Snake
{
    Int length;
    Dir direction;
    Pos positions[EVIL_SNAKE_MAX_LENGTH];
} Evil_Snake;


#define GAME_STATE1_TOTAL_EVIL_SNAKES 10
typedef struct Game_State1
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
    Int evil_snake_index;
    Evil_Snake evil_snakes[GAME_STATE1_TOTAL_EVIL_SNAKES];
} Game_State1;

void game_state1_init(Game_State1 *new_g);
void evil_snake_move(Evil_Snake *snake, World_State0 *w);
void evil_snake_draw(const Evil_Snake *snake, const World_State0 *w);
// returns true on collision
bool evil_snake_player_collision_logic(const Evil_Snake *snake, const Player *player);

Level_Return game_state1_frame0(Game_State1 *g);
