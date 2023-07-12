#pragma once
#include "very_general.h"

#define TOTAL_LEVELS 26

#define LEVEL_DATA_DEATH_WAIT_TIME 0.3f
typedef struct
{
    Level l;
    Int level_num; // what level we are at
    double time_of_level_start;
    Int deaths_in_level;
	// for showing a short cutscene after dying
	float death_wait_timer;
} Level_Data;

void level_init(Level *l, const Int frame);

void level_data_init(Level_Data *ld, Int level_num);
// handles just resetting and stuff, returns whether level was completed or not
// DOES NOT FREE ANYTHING, just handles resets and the like
Level_Return level_run_correctly(Level *l);

// gives the score depending on how long the player took to complete the level
Int level_data_get_score(Level_Data *ld);
