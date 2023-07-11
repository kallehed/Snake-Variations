#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "level_declarations.h"
#include "main.h"
#include "very_general.h"

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static const Set_Level_Code LEVEL_SET_FUNCS[] = {
    level_set_First,
    level_set_BlueSnakes,
    level_set_Skin,
    level_set_Boxes,
    level_set_EverGrowing,
    level_set_GigFreeFast,
    level_set_HidingBoxes,
    level_set_YouFood,
    level_set_Maze,
    level_set_GetSmall,
    level_set_StaticPlatformer,
    level_set_Seeker,
    level_set_UnSync,
    level_set_Spinny,
    level_set_OpenWorld,
    level_set_Wait,
    level_set_Suicide,
    level_set_Attack,
    level_set_Accel,
    level_set_Zelda,
    level_set_InZoom,
    level_set_ObsCourse,
    level_set_YouBlue,
    level_set_FallFood,
    level_set_OnceMaze,
};

static void level_init(Level *l, const Int frame);

// handles just resetting and stuff, returns whether level was completed or not
// DOES NOT FREE ANYTHING, just handles resets and the like
static Level_Return level_run_correctly(Level *l)
{
    if (DEV) // hacks
    {
        if (IsKeyPressed(KEY_N))
            return Level_Return_Next_Level;
    }

    switch (l->frame_code(l->_data))
    {
    case Level_Return_Continue: {
        if (IsKeyPressed(KEY_R))
        {
            goto GOTO_RESET_LEVEL;
        }
        return Level_Return_Continue;
    }
    break;
    case Level_Return_Next_Level: {
        TraceLog(LOG_INFO, "%s", "Go To Next Level\n");
        return Level_Return_Next_Level;
    }
    break;
    case Level_Return_Reset_Level: {
    GOTO_RESET_LEVEL:
        l->init_code(l->_data);
        return Level_Return_Reset_Level;
    }
    break;
    }
    return Level_Return_Reset_Level; // control flow CANT go here
}

static Surprise_State surprise_init(void)
{
    Surprise_State s;
    s.inputs_made = 0;
    s.start_time = GetTime();

    return s;
}

static Death_Stats_State death_stats_init(Int deaths_in_level, Int total_deaths, Int evilness)
{
    Death_Stats_State d;
    d.start_time = GetTime();
    d.deaths_in_level = deaths_in_level;
    d.total_deaths = total_deaths;
    d.evilness = evilness;
    return d;
}

// returns true if level is done
static bool game_handle_level(Game *g)
{
    switch (level_run_correctly(&g->l))
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
            const Int wait_time = (DEV) ? 500 : 1500;
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
        g->frame++;
        g->deaths_in_level = 0;
        level_init(&g->l, g->frame);
        return true;
    }
    break;
    case Level_Return_Reset_Level: {
        g->global_deaths++;
        g->deaths_in_level++;

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
            g->death_stats = death_stats_init(g->deaths_in_level, g->global_deaths, g->global_evilness);
        }
    }
    break;
    }
    // printf("deaths: %d\n", g->deaths);
    return false;
}

// returns true if we should exit surprise state
static bool game_surprise(Surprise_State *surp)
{
    Color background_color = BLACK;
    Color lose_color = RED;
    const double time_passed = GetTime() - surp->start_time;

    if (time_passed > 9.5)
    {
        background_color = RED;
    }
    if (time_passed > 10.0) // go back
    {
        return true;
    }

    if (get_dir_from_input() != Dir_Nothing)
    {
        surp->inputs_made++;
        lose_color = GOLD;
    }

    BeginDrawing();
    ClearBackground(background_color);
    DrawText("STOP!", 50, 100, 250, WHITE);
    DrawText("Don't touch anything!", 100, 350, 60, YELLOW);
    {
        char text[100];
        snprintf(text, sizeof(text), "You lose %d points!", surp->inputs_made);
        DrawText(text, 150, 450, 30, lose_color);
    }

    {
        double rect_y = -300.0 + 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle(0, (int)rect_y, WINDOW_WIDTH, 100, (Color){0, 255, 0, 150});
    }
    {
        double rect_x = 1200 - 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle((int)rect_x, 0, 100, WINDOW_HEIGHT, (Color){0, 255, 0, 150});
    }
    {
        double rect_y = 1700.0 - 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle(0, (int)rect_y, WINDOW_WIDTH, 100, (Color){0, 255, 0, 150});
    }
    {
        double rect_x = -1500 + 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle((int)rect_x, 0, 100, WINDOW_HEIGHT, (Color){0, 255, 0, 150});
    }

    EndDrawing();
    return false;
}

static Cutscene_State cutscene_init(Int points_gained, Int global_score, Int nr)
{
    Cutscene_State c;
    c.start_time = GetTime();
    c.points_gained = points_gained;
    c.global_score = global_score;
    c.cutscene_nr = nr;
    return c;
}

// returns true if cutscene is done
static bool game_cutscene(Cutscene_State *cs)
{
    const double time_passed = GetTime() - cs->start_time;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(time_passed * 200 - 1300, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }

    {
        const char *const texts[] = {
            "You have come so far already!",
            "Truly great feats achieved!",
            "Wow! Beautifully played!!",
            "Boxtravagantly played right there!",
            "Wow, that level couldn't get any longer!",
            "Couldn't see you there!",
            "Boxceptionally manoeuvred right there!",
            "Delightfully inverted, or?",
            "That maze sure wasn't prepared for that!",
            "Reversion is not diversion.",
            "Mario and Luigi surely must look up to you!",
            "WARNING! The greatest player ever is here!",
            "Trippy! Surely unsurreptitiously moved!",
            "Truly flabbergasting! The horror!",
            "Woooow, such great plains!",
            "Waiting? Never heard of her",
            "Disco floor sure is getting crowded!",
            "Hmph, AAAhh! Survival to be sure!",
            "Weeeeeee, snake to the moon!",
            "You are Slink, saviour of snakekind!",
            "Where did the camera-man go? Hello?",
            "Ping pang, bing dang! Lots happening!",
            "The universe is contracting onto itself",
            "Evil escalators sure escape my every ace!",
            "Wow haha, levels sure are cool now!",
        };
        const char *const my_text = texts[cs->cutscene_nr];

        const Int my_text_len = TextLength(my_text);

        Int char_at = (int32_t)(12.0 * (time_passed - 2.0));
        if (char_at < 0)
            char_at = 0;

        char buffer[200];
        // first give the buffer the personal cutscene text
        snprintf(buffer, sizeof(buffer) - 1, "%s", my_text); // spare a character for \0
        buffer[sizeof(buffer) - 1] = '\0';

        const bool draw_score = char_at >= my_text_len;

        if (!draw_score)
        {
            buffer[char_at] = '\0';
        }

        DrawText(buffer, 35, 50, 30, BLACK);

        if (draw_score)
        {
            char_at -= my_text_len + 14;
            if (char_at < 0)
                char_at = 0;

            snprintf(buffer, sizeof(buffer) - 1, "\nPoints recieved: %d     \nTotal Points: %d", cs->points_gained,
                     cs->global_score);
            if (char_at < (Int)sizeof(buffer))
            {
                buffer[char_at] = '\0';
            }

            DrawText(buffer, 35, 50, 30, BLACK);
        }
    }
    EndDrawing();
    if (time_passed > 12.0)
        return true;
    return false;
}

static bool death_stats_frame(Death_Stats_State *ds)
{
    const double time_passed = GetTime() - ds->start_time;

    BeginDrawing();
    ClearBackground(BLACK);

    const double time_til_first_text = 2.0;
    bool exit_this_cutscene = false;
    if (time_passed > time_til_first_text)
    {
        char text[] = "DEATH";
        Int char_at = (Int)(10.0 * (time_passed - time_til_first_text));

        const Int first_text_len = sizeof(text);
        const bool at_first = char_at < first_text_len;

        if (at_first)
        {
            text[char_at] = '\0';
        }

        DrawText(text, 40, 50, 220, (Color){255, 0, 0, 255});

        if (!at_first)
        {
            char_at -= first_text_len + 20;
            if (char_at < 0)
                char_at = 0;

            char buffer[100];
            snprintf(buffer, sizeof(buffer) - 1, "Total deaths: %d", ds->total_deaths);
            buffer[sizeof(buffer) - 1] = '\0';
            const Int second_text_len = TextLength(buffer);

            const bool at_second = char_at < second_text_len;

            if (at_second)
            {
                buffer[char_at] = '\0';
            }
            DrawText(buffer, 100, 300, 40, (Color){200, 200, 0, 255});

            if (!at_second)
            {
                char_at -= second_text_len + 5;
                if (char_at < 0)
                    char_at = 0;
                snprintf(buffer, sizeof(buffer) - 1, "Total evilness: %d", ds->evilness);
                buffer[sizeof(buffer) - 1] = '\0';
                const Int third_text_len = TextLength(buffer);
                const bool at_third = char_at < third_text_len;
                if (at_third)
                {
                    buffer[char_at] = '\0';
                }
                DrawText(buffer, 150, 365, 30, VIOLET);

                if (!at_third)
                {
                    char_at -= third_text_len;

                    if (char_at > 5)
                    {
                        ClearBackground((Color){100, 0, 0, 200});
                    }
                    if (char_at > 8)
                    {
                        exit_this_cutscene = true;
                    }
                }
            }
        }
    }
    EndDrawing();

    if (exit_this_cutscene)
        return true;

    return false;
}

static void game_frame(Game *g)
{
    switch (g->game_mode)
    {
    case Game_Mode_Level: {
        if (game_handle_level(g))
        {
            g->game_mode = Game_Mode_Cutscene;
            double time_passed = GetTime() - g->time_of_level_start;
            double score = 100.0 / (sqrt(0.02 * time_passed + 1.0));
            Int int_score = (Int)score;
            printf("time passed: %f, score: %f, int_score: %d\n", time_passed, score, int_score);
            g->global_score += int_score;
            g->cut = cutscene_init(int_score, g->global_score, g->frame - 1);
        }
    }
    break;
    case Game_Mode_Random_Surprise: {
        if (game_surprise(&g->surp))
        {
            g->game_mode = Game_Mode_Level;
            g->global_score -= g->surp.inputs_made;
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
        if (game_cutscene(&g->cut))
        {
            g->game_mode = Game_Mode_Level;
            g->time_of_level_start = GetTime();
            g->deaths_in_level = 0;
        }
    }
    break;
    }
}

#ifdef TEST_ALL_LEVELS
static void test_all_levels(void)
{
    SetTargetFPS(60); // DONT CHANGE, otherwise less will be seen
    for (int frame = 0;; frame += 2)
    {
        Level l = level_init(frame);

        if (NULL == l.frame_code)
        {
            break;
        }

        l.init_code(l._data);

        for (int runs = 0; runs < 240; ++runs)
        {
            meta_game_run_level_correctly(&l);
        }
        free(l._data);
        l._data = NULL;
    }
}
#endif

// sets frame to -1 if the frame doesn't have a valid level, frees previous level that was there
static void level_init(Level *l, const Int frame)
{
    free(l->_data);
    l->_data = NULL;

    unsigned int at = frame;
    if (at >= (sizeof(LEVEL_SET_FUNCS) / sizeof(Set_Level_Code)))
    {
        printf("--------\n");
        printf("VERY BAD DEATH!!!!!!!!! AHHHHHHHH LEVEL NOT EXIST\n");
        printf("--------\n");
        // at = 0;
        l->size = 0;
        l->_data = NULL;
        l->frame_code = NULL;
        l->init_code = NULL;
        // l.frame = -1;
    }
    LEVEL_SET_FUNCS[at](l);

    // INIT
    printf("Mallocing size: %u\n", l->size);
    l->_data = malloc(l->size);
    if (NULL == l->_data) // Check for Out Of Memory
        puts("!!!!!!!!! OUT OF MEMORY, MALLOC RETURNED NULL!!!!!!!!");

    l->init_code(l->_data);
}

static Game game_init(void)
{
    Game g;
    if (DEV)
    {
        g.frame = 4; // 24 latest
    }
    else
    {
        g.frame = 0;
    }
    g.l._data = NULL;
    level_init(&g.l, g.frame);
    g.global_score = 0;
    g.time_of_level_start = GetTime();
    g.game_mode = Game_Mode_Level;
    g.try_surprise_timer = 0.0f;
    g.time_of_prev_surprise = GetTime();
	g.time_of_prev_death_stats = GetTime();
    g.global_deaths = 0;
    g.deaths_in_level = 0;
    return g;
}

static void game_loop(void)
{
    Game g = game_init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg((void (*)(void *))game_frame, &g, 0, 1);
#else

    SetTargetFPS(GAME_FPS); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        game_frame(&g);
    }
    free(g.l._data);
    g.l._data = NULL;
#endif
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Variations");

#ifdef TEST_ALL_LEVELS
    test_all_levels();
#endif
    game_loop();

    CloseWindow();

    return 0;
}
