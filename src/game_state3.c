#include "game_state3.h"
#include <stdlib.h>
#include "level_declarations.h"
// EverGrowing

void level_set_EverGrowing(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state3_frame0;
    mg->init_code = (Level_Init_Code)game_state3_init0;
    mg->size = (sizeof(Game_State3));
}

void game_state3_init0(Game_State3 *new_g)
{
    Game_State3 g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player_points = 0;
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

// ever expanding snake
Level_Return game_state3_frame0(Game_State3 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        Int player_len = g->player.length;

        g->player.length++;
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        g->player.length--;
        if (pos_equal(player_nth_position(&g->player, 0), g->food.pos))
        {
            food_init_position(&g->food, &g->player, w);
            g->player_points++;
        }
        g->player.length = player_len + 1;

        // printf("player len: %d\n", g->player.length);
    }

    Int points_left = 5 - g->player_points;
    if (points_left == 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(points_left);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
