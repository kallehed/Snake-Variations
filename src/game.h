#pragma once

#include "game_cutscenes.h"
#include "game_state_Ending.h"
#include "very_general.h"
#include "music.h"

typedef enum
{
    // Actually playing
    Game_Mode_Level,
    // Randomly entered when playing
    Game_Mode_Random_Surprise,
    // Randomly entered after dying
    Game_Mode_Death_Stats,
    // After winning level
    Game_Mode_Cutscene,
    // After winning the game
    Game_Mode_Ending,
} Game_Mode;

typedef struct
{
    Level_Data ld;

    Int global_score;

    float try_surprise_timer; // the random surprise is checked at intervals in time, this variable times that
    double time_of_prev_surprise;

    double time_of_prev_death_stats;

    Game_Mode game_mode;
    union { // Only one can be active, or none at all when level is playing
        Surprise_State surp;
        Cutscene_State cut;
        Death_Stats_State death_stats;
        GS_Ending gs_ending;
    };

    Int global_deaths;
    Int global_evilness;

	Music_Enum cur_music;
	Music musics[TOTAL_MUSICS];
} Game;

Game game_init(void);
void game_deinit(Game *g);
void game_run_frame(Game *g);
void game_init_next_level(Game *g);
// returns true if level is done
bool game_handle_level(Game *g);
