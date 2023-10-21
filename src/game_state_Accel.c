#include "game_state_Accel.h"
#include "level_declarations.h"
#include <math.h>

void level_set_Accel(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_frame_Accel;
    mg->init_code = (Level_Init_Code)game_state_init_Accel;
    mg->size = (sizeof(Game_State_Accel));
}

void game_state_init_Accel(Game_State_Accel *new_g, Allo *allo)
{
    Game_State_Accel g;
    g.w = world_state0_init(24);
    g.player = player_init((Pos){.x = 10, .y = 10}, 3, 100, Dir_Right, allo);
    food_init_position(&g.food, g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state_frame_Accel(Game_State_Accel *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(g->player);

    double wait_time = 0.01 + pow(sin(GetTime()), 2.0);

    if (time_move_logic_general(&g->time_for_move, wait_time))
    {
        if (player_move(g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(g->player, &g->food, w);
    }

    Int food_left_to_win = 9 - g->player->length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(VIOLET);

    draw_food_left(food_left_to_win);
    player_draw_general(g->player, SKYBLUE, BLUE, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
