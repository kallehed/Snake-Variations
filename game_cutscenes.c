#include "game_cutscenes.h"

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

