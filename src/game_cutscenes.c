#include "game_cutscenes.h"
#include <stdlib.h>

void metagame_set_level_Cutscene0(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
    mg->init_code = (Meta_Game_Init_Code)game_cutscene0_init;
    mg->size = (sizeof(Game_Cutscene0));
}

void game_cutscene0_init(Game_Cutscene0 *new_g)
{
    Game_Cutscene0 g;
    g.start_time = GetTime();
    *new_g = g;
}

Level_Return game_cutscene0_frame0(Game_Cutscene0 *g)
{
    double time_passed = GetTime() - g->start_time;

    if (time_passed > 2.5f)
        return Level_Return_Next_Level;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(time_passed * 360, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }
    EndDrawing();

    return Level_Return_Continue;
}

Level_Return game_cutscene0_frame1(Game_Cutscene0 *g)
{
    double time_passed = GetTime() - g->start_time;

    if (time_passed > 2.f)
        return Level_Return_Next_Level;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(WINDOW_WIDTH - time_passed * 500, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }
    EndDrawing();

    return Level_Return_Continue;
}
