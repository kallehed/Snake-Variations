#pragma once
#include "very_general.h"
#include "player_related.h"

#define SPLATFORMER_MAP_WIDTH 30
#define SPLATFORMER_MAP_HEIGHT 22
#define SPLATFORMER_MAX_FOODS 5

typedef int8_t SPlatformer_Block;
#define SPlatformer_Block_Air 0
#define SPlatformer_Block_Solid 1

typedef struct {
    World_State0 w;
    Player player;
    Food foods[SPLATFORMER_MAX_FOODS];
    double time_for_move;
	bool turn;

	SPlatformer_Block map[SPLATFORMER_MAP_HEIGHT][SPLATFORMER_MAP_WIDTH];

} Game_State_StaticPlatformer;

void game_state_init_StaticPlatformer(Game_State_StaticPlatformer *new_g); 

Level_Return game_state_frame_StaticPlatformer(Game_State_StaticPlatformer *g);
