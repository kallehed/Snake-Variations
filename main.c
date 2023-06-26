#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_cutscenes.h"
#include "game_state0.h"
#include "game_state1.h"
#include "game_state2.h"
#include "game_state3.h"
#include "game_state4.h"
#include "game_state_Seeker.h"
#include "game_state_StaticPlatformer.h"
#include "game_state_YouFood.h"
#include "game_state_maze.h"
#include "very_general.h"

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

typedef Level_Return (*Meta_Game_Frame_Code)(void *);
typedef void (*Meta_Game_Init_Code)(void *);

typedef struct Meta_Game
{
    Meta_Game_Frame_Code frame_code;
    Meta_Game_Init_Code init_code;

    void *data;
    Int frame;
} Meta_Game;

static Meta_Game meta_game_init(Int frame)
{
    Meta_Game mg;

    if (DEV)
    {
        Int skip = 22;
        if (frame < skip)
            frame = skip;
    }
    mg.frame = frame;

    if (frame % 2 == 1)
    {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    else
    {
        switch (frame / 2)
        {
        case 0: {
            mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame0;
            mg.init_code = (Meta_Game_Init_Code)game_state0_init0;
            mg.data = malloc(sizeof(Game_State0));
        }
        break;
        case 1: {
            mg.frame_code = (Meta_Game_Frame_Code)game_state1_frame0;
            mg.init_code = (Meta_Game_Init_Code)game_state1_init;
            mg.data = malloc(sizeof(Game_State1));
        }
        break;
        case 2: {
            mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame1;
            mg.init_code = (Meta_Game_Init_Code)game_state0_init0;
            mg.data = malloc(sizeof(Game_State0));
        }
        break;
        case 3: { // boxes
            mg.frame_code = (Meta_Game_Frame_Code)game_state2_frame0;
            mg.init_code = (Meta_Game_Init_Code)game_state2_init;
            mg.data = malloc(sizeof(Game_State2));
        }
        break;
        case 4: { // ever growing
            mg.frame_code = (Meta_Game_Frame_Code)game_state3_frame0;
            mg.init_code = (Meta_Game_Init_Code)game_state3_init0;
            mg.data = malloc(sizeof(Game_State3));
        }
        break;
        case 5: { // gigantic free fast
            mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame2;
            mg.init_code = (Meta_Game_Init_Code)game_state0_init1;
            mg.data = malloc(sizeof(Game_State0));
        }
        break;
        case 6: { // ever growing
            mg.frame_code = (Meta_Game_Frame_Code)game_state4_frame0;
            mg.init_code = (Meta_Game_Init_Code)game_state4_init;
            mg.data = malloc(sizeof(Game_State4));
        }
        break;
        case 7: { // You are food
            mg.frame_code = (Meta_Game_Frame_Code)game_state_YouFood_frame;
            mg.init_code = (Meta_Game_Init_Code)game_state_YouFood_init;
            mg.data = malloc(sizeof(Game_State_YouFood));
        }
        break;
        case 8: { // You are food
            mg.frame_code = (Meta_Game_Frame_Code)game_state_Maze_frame;
            mg.init_code = (Meta_Game_Init_Code)game_state_Maze_init;
            mg.data = malloc(sizeof(Game_State_Maze));
        }
        break;
        case 9: { // GetSmall
            mg.frame_code = (Meta_Game_Frame_Code)game_state0_frameGetSmall;
            mg.init_code = (Meta_Game_Init_Code)game_state0_init_GetSmall;
            mg.data = malloc(sizeof(Game_State0));
        }
        break;
        case 10: { // static platformer
            mg.frame_code = (Meta_Game_Frame_Code)game_state_frame_StaticPlatformer;
            mg.init_code = (Meta_Game_Init_Code)game_state_init_StaticPlatformer;
            mg.data = malloc(sizeof(Game_State_StaticPlatformer));
        }
        break;
        case 11: {
            mg.frame_code = (Meta_Game_Frame_Code)game_state_frame_Seeker;
            mg.init_code = (Meta_Game_Init_Code)game_state_init_Seeker;
            mg.data = malloc(sizeof(Game_State_Seeker));
        }
        break;
        default: {
            printf("VERY BAD DEATH!!!!!!!!! AHHHHHHHH LEVEL NOT EXIST\n");
        }
        break;
        }
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
    //--------------------------------------------------------------------------------------

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

    SetTargetFPS(GAME_FPS);

    game_loop();

    CloseWindow();

    return 0;
}
