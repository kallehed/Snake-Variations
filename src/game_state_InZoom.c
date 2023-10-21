#include "game_state_InZoom.h"
#include "level_declarations.h"
#include "player_related.h"
#include "very_general.h"
#include <math.h>

void level_set_InZoom(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_frame_InZoom;
    mg->init_code = (Level_Init_Code)game_state_init_InZoom;
    mg->size = (sizeof(Game_State_InZoom));
}

void game_state_init_InZoom(Game_State_InZoom *new_g)
{
    Game_State_InZoom g;
    g.w = world_state0_init(20);
    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 1, Dir_Right);
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

// normal snake
Level_Return game_state_frame_InZoom(Game_State_InZoom *g)
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

    Int food_left_to_win = 8 - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_fps();
    {
        // TraceLog(LOG_INFO, "cam_x: %f", cam_x); // 2*width/(3)  width/ 2
        const float zoom = 3.0;
        Pos p_pos = player_nth_position(&g->player, 0);
        float cam_x = p_pos.x * w->block_pixel_len - (WINDOW_WIDTH / 2.0) / zoom;
        float cam_y = p_pos.y * w->block_pixel_len - (WINDOW_HEIGHT / 2.0) / zoom;
        cam_x = fmaxf(0.f, cam_x);
        cam_x = fminf(WINDOW_WIDTH * (1.0f - 1.0f / zoom), cam_x);
        cam_y = fmaxf(0.f, cam_y);
        cam_y = fminf(WINDOW_HEIGHT * (1.0f - 1.0f / zoom), cam_y);
        BeginMode2D(
            (Camera2D){.offset = {.x = 0, .y = 0}, .target = {.x = cam_x, .y = cam_y}, .rotation = 0, .zoom = zoom});
    }

    draw_food_left(food_left_to_win);
    player_draw(&g->player, w);
    food_draw(&g->food, w);

    EndDrawing();
    return Level_Return_Continue;
}
