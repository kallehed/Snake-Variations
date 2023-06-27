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
typedef struct 
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
    Int evil_snake_index;
    Evil_Snake evil_snakes[GAME_STATE1_TOTAL_EVIL_SNAKES];
} Game_State1;

typedef struct 
{
	Game_State1 g1;
	double evil_snake_intervals[GAME_STATE1_TOTAL_EVIL_SNAKES];
	double evil_snake_time_for_moves[GAME_STATE1_TOTAL_EVIL_SNAKES];
} Game_State1_UnSync;

// void evil_snake_move(Evil_Snake *snake, World_State0 *w);
// void evil_snake_draw(const Evil_Snake *snake, const World_State0 *w);
// returns true on collision
// bool evil_snake_player_collision_logic(const Evil_Snake *snake, const Player *player);

void game_state1_init(Game_State1 *new_g);
void game_state1_init_UnSync(Game_State1_UnSync *new_g);
Level_Return game_state1_frame0(Game_State1 *g);
Level_Return game_state1_frame_UnSync(Game_State1_UnSync *g);
