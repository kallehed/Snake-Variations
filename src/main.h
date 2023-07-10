#pragma once

#include "very_general.h"

typedef enum
{
    // actually playing
    Game_Mode_Level,
    // randomly entered when playing
    Game_Mode_Random_Surprise,
    // randomly entered after dying
    Game_Mode_Death_Comfort,
    //
    Game_Mode_Cutscene,
} Game_Mode;

typedef struct
{
    double start_time;
    Int inputs_made;

} Surprise_State;

typedef struct
{
    double start_time;
	Int points_gained;
	Int global_score;
	Int cutscene_nr;
} Cutscene_State;

typedef struct
{
    Int frame; // what level we are at
    Level l;

	Int global_score;

    Game_Mode game_mode;
    float try_surprise_timer; // the random surprise is checked at intervals in time, this variable times that
    double time_of_prev_surprise;
    Surprise_State surp;

    Cutscene_State cut;

    Int deaths;
} Game;
