#include "game_state_OpenWorld.h"
#include "player_related.h"
#include "raylib.h"
#include "very_general.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void game_state_init_OpenWorld(Game_State_OpenWorld *new_g)
{
    Game_State_OpenWorld g;
    g.w = world_state0_init_general(100, 100, 20);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = 5, .y = 5};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}
// normal snake
Level_Return game_state_frame_OpenWorld(Game_State_OpenWorld *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Level_Return food_left_to_win = (DEV ? 6 : 6) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    {
        Pos p_pos = player_nth_position(&g->player, 0);
        const float max_x = (float)w->block_pixel_len * (float)w->width - (float)WINDOW_WIDTH;
        const float max_y = (float)w->block_pixel_len * (float)w->height - (float)WINDOW_HEIGHT;
        float cam_x = fminf(max_x, fmaxf(0.f, (float)(p_pos.x * w->block_pixel_len - WINDOW_WIDTH / 2)));
        float cam_y = fminf(max_y, fmaxf(0.f, (float)(p_pos.y * w->block_pixel_len - WINDOW_HEIGHT / 2)));

        // printf("cam_x: %f, max_x: %f\n", cam_x, max_x);
        Camera2D s = (Camera2D){
            .offset = {.x = 0.f, .y = 0.f}, .target = {.x = cam_x, .y = cam_y}, .rotation = 0.f, .zoom = 1.f};
        BeginMode2D(s);
    }

    ClearBackground(RAYWHITE);
    draw_food_left_in_2D_space(food_left_to_win, w->width * w->block_pixel_len, w->height * w->block_pixel_len);

    player_draw(&g->player, w);
    food_draw(&g->food, w);

    EndMode2D();
    draw_fps();
    EndDrawing();

    return Level_Return_Continue;
}
