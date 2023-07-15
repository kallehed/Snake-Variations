#include "game.h"
#include "level.h"
#include "music.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Game game_init(void)
{
    Game g;
    Int start_level_num = 0;
    if (DEV)
    {
        start_level_num = 0; // 25 latest
    }
    g.ld.l._data = NULL;
    level_data_init(&g.ld, start_level_num);
    g.global_score = 0;
    g.game_mode = Game_Mode_Level;
    g.try_surprise_timer = 0.0f;
    g.time_of_prev_surprise = GetTime();
    g.time_of_prev_death_stats = GetTime();
    g.global_deaths = 0;
    g.global_evilness = 0;

    g.cur_music = 0;

    // Music
    {
        static const char *filenames[] = {
            "f/music/snake_basic.ogg", "f/music/snake_base_headed.ogg", "f/music/cutscene_magical.ogg",
            "f/music/snake_weird.ogg", "f/music/snake_drummy.ogg",      "f/music/snake_scary.ogg",
            "f/music/snake_doom.ogg",  "f/music/snake_celeb.ogg",       "f/music/snake_mess.ogg",
            "f/music/snake_final.ogg", "f/music/snake_ending.ogg",      "f/music/snake_surprise.ogg",
            "f/music/snake_death.ogg",
        };
        _Static_assert(sizeof(filenames) / sizeof(filenames[0]) == TOTAL_MUSICS,
                       "Must provide correct number of files for musics!");
        for (Int i = 0; i < TOTAL_MUSICS; ++i)
        {
            g.musics[i] = LoadMusicStream(filenames[i]);
            PlayMusicStream(g.musics[i]);
        }
        g.musics[Music_Cutscene_Magical].looping = false;

        g.cur_music = Music_Snake_Basic;
    }
    // Sound
    {
        static const char *filenames[] = {
            "f/sound/snake_sound_die.ogg",
            "f/sound/snake_sound_eat2.ogg",
        };
        _Static_assert(sizeof(filenames) / sizeof(filenames[0]) == TOTAL_SOUNDS,
                       "Must provide correct number of files for sounds!");
        for (Int i = 0; i < TOTAL_SOUNDS; ++i)
        {
            g.sounds[i] = LoadSound(filenames[i]);
        }
    }

    return g;
}

void game_deinit(Game *g)
{
    free(g->ld.l._data);
    g->ld.l._data = NULL;
    for (Int i = 0; i < TOTAL_MUSICS; ++i)
    {
        UnloadMusicStream(g->musics[i]);
    }
    for (Int i = 0; i < TOTAL_SOUNDS; ++i)
    {
        UnloadSound(g->sounds[i]);
    }
}

void game_run_frame(Game *g)
{
    switch (g->game_mode)
    {
    case Game_Mode_Level: {
        if (game_handle_level(g))
        {
            game_init_cutscene(g);
        }
    }
    break;
    case Game_Mode_Random_Surprise: {
        UpdateMusicStream(g->musics[Music_Snake_Surprise]);
        if (surprise_frame(&g->surp))
        {
            g->game_mode = Game_Mode_Level;
            g->global_score -= g->surp.points_lost;
        }
    }
    break;
    case Game_Mode_Death_Stats: {
        UpdateMusicStream(g->musics[Music_Snake_Death]);
        if (death_stats_frame(&g->death_stats))
        {
            g->game_mode = Game_Mode_Level;
        }
    }
    break;
    case Game_Mode_Cutscene: {
        UpdateMusicStream(g->musics[g->cur_music]); // Update music buffer with new stream data
        if (cutscene_frame(&g->cut))
        {
            game_init_next_level(g);
        }
    }
    break;
    case Game_Mode_Ending: {
        UpdateMusicStream(g->musics[g->cur_music]);
        gs_frame_Ending(&g->gs_ending);
    }
    break;
    }
}

void game_init_next_level(Game *g)
{
    // For final Ending cutscene
    if (g->ld.level_enum >= (TOTAL_LEVELS - 1))
    {
        g->game_mode = Game_Mode_Ending;
        g->cur_music = Music_Snake_Ending;
        g->gs_ending = gs_init_Ending(g->global_score, g->global_deaths, g->global_evilness);
    }
    else
    // Next level
    {
        g->ld.level_enum++;
        g->cur_music = level_get_music(g->ld.level_enum);
        g->game_mode = Game_Mode_Level;
        level_data_init(&g->ld, g->ld.level_enum);
    }
}

// returns true if level is done
bool game_handle_level(Game *g)
{
    if (g->ld.death_wait_timer > 0.f)
    {
        g->ld.death_wait_timer -= GetFrameTime();
        BeginDrawing();

        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    else
    {
        UpdateMusicStream(g->musics[g->cur_music]); // Update music buffer with new stream data
        switch (level_run_correctly(&g->ld.l))
        {
        case Level_Return_Continue: {
            // possibly start surprise easter egg thingy
            game_logic_for_init_surprise_cutscene(g);
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

            PlaySound(g->sounds[Sound_Snake_Die]);

            const double time_since_death_stats = GetTime() - g->time_of_prev_death_stats;
            const int int_time = (int)time_since_death_stats;
            const int max_wait = GAME_DEATH_STATS_CUTSCENE_WAIT_TIME;
            const bool should_show =
                (int_time >= max_wait) || (GetRandomValue((int)time_since_death_stats, max_wait) == max_wait);
            printf("SHOULD DEATH STATS? time passed: %f, max_wait: %d\n", time_since_death_stats, max_wait);

            if (should_show)
            {
                game_init_death_stats_cutscene(g);
            }
            else
            {
                g->ld.death_wait_timer = LEVEL_DATA_DEATH_WAIT_TIME;
            }
        }
        break;
        }
    }
    // printf("deaths: %d\n", g->deaths);
    return false;
}

void game_init_cutscene(Game *g)
{
    g->game_mode = Game_Mode_Cutscene;
    Int add_score = level_data_get_score(&g->ld);
    g->global_score += add_score;
    g->cur_music = Music_Cutscene_Magical;
    StopMusicStream(g->musics[g->cur_music]);
    PlayMusicStream(g->musics[g->cur_music]);
    g->cut = cutscene_init(add_score, g->global_score, g->ld.level_enum);
}

void game_logic_for_init_surprise_cutscene(Game *g)
{
    g->try_surprise_timer += GetFrameTime();
    const float surprise_check_interval = 1.f; // every second
    if (surprise_check_interval <= g->try_surprise_timer)
    {
        g->try_surprise_timer -= surprise_check_interval;
        const double cur_time = GetTime() * (DEV ? (1.0) : 1.0);
        const double time_since_surprise = cur_time - g->time_of_prev_surprise;
        const Int wait_time = GAME_SURPRISE_CUTSCENE_WAIT_TIME;
        if ((Int)(time_since_surprise) > wait_time || wait_time == GetRandomValue((Int)time_since_surprise, wait_time))
        {
            printf("yoo this is rand! %f\n", time_since_surprise);
            g->time_of_prev_surprise = GetTime();
            g->game_mode = Game_Mode_Random_Surprise;
            g->surp = surprise_init();
        }
    }
}

void game_init_death_stats_cutscene(Game *g)
{
    g->time_of_prev_death_stats = GetTime();
    g->game_mode = Game_Mode_Death_Stats;
    StopMusicStream(g->musics[Music_Snake_Death]);
    PlayMusicStream(g->musics[Music_Snake_Death]);
    g->global_evilness += GetRandomValue(13, 42);
    g->death_stats = death_stats_init(g->ld.deaths_in_level, g->global_deaths, g->global_evilness);
}
