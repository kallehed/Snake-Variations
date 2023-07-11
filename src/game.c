#include "game.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

Game game_init(void)
{
    Game g;
    Int start_level_num = 0;
    if (DEV)
    {
        start_level_num = 0; // 24 latest
    }
    g.ld.l._data = NULL;
    level_data_init(&g.ld, start_level_num);
    g.global_score = 0;
    g.game_mode = Game_Mode_Level;
    g.try_surprise_timer = 0.0f;
    g.time_of_prev_surprise = GetTime();
    g.time_of_prev_death_stats = GetTime();
    g.global_deaths = 0;
    return g;
}

void game_run_frame(Game *g)
{
    switch (g->game_mode)
    {
    case Game_Mode_Level: {
        if (game_handle_level(g))
        {
            g->game_mode = Game_Mode_Cutscene;
            Int add_score = level_data_get_score(&g->ld);
            g->global_score += add_score;
            g->cut = cutscene_init(add_score, g->global_score, g->ld.level_num);
        }
    }
    break;
    case Game_Mode_Random_Surprise: {
        if (surprise_frame(&g->surp))
        {
            g->game_mode = Game_Mode_Level;
            g->global_score -= g->surp.points_lost;
        }
    }
    break;
    case Game_Mode_Death_Stats: {
        if (death_stats_frame(&g->death_stats))
        {
            g->game_mode = Game_Mode_Level;
        }
    }
    break;
    case Game_Mode_Cutscene: {
        if (cutscene_frame(&g->cut))
        {
            g->game_mode = Game_Mode_Level;
            level_data_init(&g->ld, g->ld.level_num + 1);
        }
    }
    break;
    }
}

// returns true if level is done
bool game_handle_level(Game *g)
{
    switch (level_run_correctly(&g->ld.l))
    {
    case Level_Return_Continue: {
        // possibly start surprise easter egg thingy

        g->try_surprise_timer += GetFrameTime();
        const float surprise_check_interval = 1.f; // every second
        if (surprise_check_interval <= g->try_surprise_timer)
        {
            g->try_surprise_timer -= surprise_check_interval;
            const double cur_time = GetTime() * (DEV ? (1.0) : 1.0);
            const double time_since_surprise = cur_time - g->time_of_prev_surprise;
            const Int wait_time = (DEV) ? 850 : 850;
            if ((Int)(time_since_surprise) > wait_time ||
                wait_time == GetRandomValue((Int)time_since_surprise, wait_time))
            {
                printf("yoo this is rand! %f\n", time_since_surprise);
                g->time_of_prev_surprise = GetTime();
                g->game_mode = Game_Mode_Random_Surprise;
                g->surp = surprise_init();
            }
        }
    }
    break;
    case Level_Return_Next_Level: {
        // level is done, next can start
        return true;
    }
    break;
    case Level_Return_Reset_Level: {
        g->global_deaths++;
        g->ld.deaths_in_level++;

        const double time_since_death_stats = GetTime() - g->time_of_prev_death_stats;
        const int int_time = (int)time_since_death_stats;
        const int max_wait = 400;
        const bool should_show =
            (int_time >= max_wait) || (GetRandomValue((int)time_since_death_stats, max_wait) == max_wait);
        printf("SHOULD DEATH STATS? time passed: %f, max_wait: %d\n", time_since_death_stats, max_wait);

        if (should_show)
        {
            g->time_of_prev_death_stats = GetTime();
            g->game_mode = Game_Mode_Death_Stats;
            g->global_evilness += GetRandomValue(13, 42);
            g->death_stats = death_stats_init(g->ld.deaths_in_level, g->global_deaths, g->global_evilness);
        }
    }
    break;
    }
    // printf("deaths: %d\n", g->deaths);
    return false;
}
