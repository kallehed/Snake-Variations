#pragma once

#include "game_cutscenes.h"
#include "game_state_Ending.h"
#include "very_general.h"
#include "music.h"

#define GAME_SURPRISE_CUTSCENE_WAIT_TIME ((DEV)? 850 : 850)
#define GAME_DEATH_STATS_CUTSCENE_WAIT_TIME 400

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

	Int cheat_counter;

	Music_Enum cur_music;
	Music musics[TOTAL_MUSICS];

	Sound sounds[TOTAL_SOUNDS];
} Game;

Game game_init(void);
void game_deinit(Game *g);
void game_run_frame(Game *g);
void game_init_next_level(Game *g);
// returns true if level is done
bool game_handle_level(Game *g);

// also adds up score from the level and the like
void game_init_cutscene(Game *g);

void game_logic_for_init_surprise_cutscene(Game *g);

void game_init_death_stats_cutscene(Game *g);


