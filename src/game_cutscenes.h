#pragma once

#include "very_general.h"
#include "level.h"

typedef struct
{
    double start_time;
    Int points_gained;
    Int global_score;
    Int cutscene_nr;
} Cutscene_State; // Inbetween levels

typedef struct
{
    double start_time;
    Int deaths_in_level;
    Int total_deaths;
    Int evilness;
} Death_Stats_State; // Says how many times you have died

typedef struct
{
    double start_time;
    Int points_lost;
} Surprise_State; // Makes you lose points if you try to move

Cutscene_State cutscene_init(Int points_gained, Int global_score, Int cutscene_nr);
bool cutscene_frame(Cutscene_State *cs);

Death_Stats_State death_stats_init(Int deaths_in_level, Int total_deaths, Int evilness);
bool death_stats_frame(Death_Stats_State *ds);

Surprise_State surprise_init(void);
bool surprise_frame(Surprise_State *surp);
