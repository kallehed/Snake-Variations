#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "level_declarations.h"

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static const Meta_Game_Set_Level_Code SET_LEVEL_FUNCS[] = {
    metagame_set_level_First,       metagame_set_level_BlueSnakes,       metagame_set_level_Skin,
    metagame_set_level_Boxes,       metagame_set_level_EverGrowing,      metagame_set_level_GigFreeFast,
    metagame_set_level_HidingBoxes, metagame_set_level_YouFood,          metagame_set_level_Maze,
    metagame_set_level_GetSmall,    metagame_set_level_StaticPlatformer, metagame_set_level_Seeker,
    metagame_set_level_UnSync,      metagame_set_level_Spinny,           metagame_set_level_OpenWorld,
    metagame_set_level_Wait,        metagame_set_level_Suicide};

static Meta_Game meta_game_init(Int frame);

#ifdef TEST_ALL_LEVELS
static void test_all_levels(void)
{
    SetTargetFPS(800);
    for (int frame = 0;; frame += 2)
    {
        Meta_Game mg = meta_game_init(frame);

        if (-1 == mg.frame)
        {
            break;
        }

        mg.init_code(mg.data);

        for (int runs = 0; runs < 60; ++runs)
        {
            mg.frame_code(mg.data);
        }
        free(mg.data);
    }
}
#endif

// sets frame to -1 if the frame doesn't have a valid level
static Meta_Game meta_game_init(Int frame)
{
    Meta_Game mg;

    if (DEV)
    {
        Int skip = 32; // 32 latest
        if (frame < skip)
            frame = skip;
    }
    mg.frame = frame;

    if (frame % 2 == 1)
    {
        metagame_set_level_Cutscene0(&mg);
    }
    else
    {
        unsigned int at = frame / 2;
        if (at >= (sizeof(SET_LEVEL_FUNCS) / sizeof(Meta_Game_Set_Level_Code)))
        {
            printf("--------\n");
            printf("VERY BAD DEATH!!!!!!!!! AHHHHHHHH LEVEL NOT EXIST\n");
            printf("--------\n");
            // at = 0;
            mg.data = NULL;
            mg.frame_code = NULL;
            mg.init_code = NULL;
            mg.frame = -1;
            return mg;
        }
        SET_LEVEL_FUNCS[at](&mg);
    }
    // INIT
    mg.init_code(mg.data);

    return mg;
}

static void meta_game_frame(Meta_Game *mg)
{
    switch (mg->frame_code(mg->data))
    {
    case Level_Return_Continue: {
        if (IsKeyPressed(KEY_R))
        {
            goto GOTO_RESET_LEVEL;
        }
    }
    break;
    case Level_Return_Next_Level: {
        TraceLog(LOG_INFO, "%s", "THE FUNCTION RETURNED ONE ONE ONE \n");
        free(mg->data);
        *mg = meta_game_init(mg->frame + 1);
    }
    break;
    case Level_Return_Reset_Level: {
    GOTO_RESET_LEVEL:
        mg->init_code(mg->data);
    }
    break;
    }
}

static void game_loop(void)
{
    Meta_Game mg = meta_game_init(0);
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg((void (*)(void *))meta_game_frame, &mg, 0, 1);
#else

    SetTargetFPS(GAME_FPS); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        meta_game_frame(&mg);
    }
#endif
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CSnake");

#ifdef TEST_ALL_LEVELS
    test_all_levels();
#endif
    game_loop();

    CloseWindow();

    return 0;
}
