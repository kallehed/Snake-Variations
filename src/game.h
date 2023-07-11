#pragma once

#include "game_cutscenes.h"
#include "very_general.h"

typedef enum
{
    // actually playing
    Game_Mode_Level,
    // randomly entered when playing
    Game_Mode_Random_Surprise,
    // randomly entered after dying
    Game_Mode_Death_Stats,
    //
    Game_Mode_Cutscene,
} Game_Mode;


typedef struct
{
    Level_Data ld;

    Int global_score;

    Game_Mode game_mode;
    float try_surprise_timer; // the random surprise is checked at intervals in time, this variable times that
    double time_of_prev_surprise;
    Surprise_State surp;

    Cutscene_State cut;

    double time_of_prev_death_stats;
    Death_Stats_State death_stats;

    Int global_deaths;
    Int global_evilness;
} Game;

Game game_init(void);
void game_run_frame(Game *g);
// returns true if level is done
bool game_handle_level(Game *g);
