#pragma once
#include "very_general.h"

typedef struct Game_Cutscene0
{
    double start_time;

} Game_Cutscene0;
void game_cutscene0_init(Game_Cutscene0 *new_g);
Level_Return game_cutscene0_frame0(Game_Cutscene0 *g);
Level_Return game_cutscene0_frame1(Game_Cutscene0 *g);
