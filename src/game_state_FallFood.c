#include "game_state_FallFood.h"
#include "level_declarations.h"
#include "maze_stuff.h"
#include "player_related.h"
#include "very_general.h"

#include <math.h>
#include <stddef.h>

void level_set_FallFood(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_frame_FallFood;
    mg->init_code = (Level_Init_Code)game_state_init_FallFood;
    mg->size = (sizeof(GS_FallFood));
}

void game_state_init_FallFood(GS_FallFood *new_g, Allo *allo)
{
    GS_FallFood g;
    g.w = world_state0_init_general(GS_FALLFOOD_WIDTH, GS_FALLFOOD_HEIGHT, WINDOW_WIDTH / GS_FALLFOOD_WIDTH);
    g.player = player_init((Pos){5, GS_FALLFOOD_HEIGHT - 5}, GS_FALLFOOD_PLAYER_START_LENGTH, 100, Dir_Right, allo);
    food_init_position(&g.food, g.player, &g.w);
    g.food = (Food){.pos = (Pos){.x = 9, GS_FALLFOOD_HEIGHT - 15}};
    g.time_for_move = 1.0;

    g.cam_y = GS_FALLFOOD_HEIGHT * g.w.block_pixel_len - WINDOW_HEIGHT;
    g.cam_y_speed = -100.f;

    for (Int i = 0; i < GS_FALLFOOD_HEIGHT; ++i)
    {
        for (Int j = 0; j < GS_FALLFOOD_WIDTH; ++j)
        {
            g.maze[i][j] = Maze0_Cell_Empty;
            if (i % 2 == 0)
            {
                if ((i / 2 + j) % 2 == 0)
                {
                    if (GetRandomValue(1, i / 300) == 1)
                    {
                        g.maze[i][j] = Maze0_Cell_Wall;
                    }
                }
            }
            if (i == 0)
            {
                g.maze[i][j] = Maze0_Cell_Wall;
            }
        }
    }

    *new_g = g;
}

// normal snake
Level_Return game_state_frame_FallFood(GS_FallFood *g)
{
    const World_State0 *const w = &g->w;
    // logic
    player_set_direction_from_input(g->player);

    if (time_move_logic_general(&g->time_for_move, 0.09))
    {
        if (maze0_player_move((Maze0_Cell *)g->maze, GS_FALLFOOD_WIDTH, g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }
        if (pos_equal(player_nth_position(g->player, 0), g->food.pos))
        {
            ++g->player->length;
            g->food.pos = (Pos){.x = GetRandomValue(0, GS_FALLFOOD_WIDTH - 1),
                                .y = (player_nth_position(g->player, 0).y - 20 + GetRandomValue(-5, 5)) / 2 * 2 + 1};
        }
    }

    // die by going offscreen
    {
        const Pos p_pos = player_nth_position(g->player, 0);
        if ((p_pos.y + 1) * w->block_pixel_len < g->cam_y ||
            (p_pos.y - 1) * w->block_pixel_len > g->cam_y + WINDOW_HEIGHT)
        {
            return Level_Return_Reset_Level;
        }
    }

    const Int food_left_to_win = GS_FALLFOOD_TOTAL_FOOD_TO_GET + GS_FALLFOOD_PLAYER_START_LENGTH - g->player->length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, WINDOW_WIDTH, 10, RED);
    DrawRectangle(0, WINDOW_HEIGHT - 10, WINDOW_WIDTH, 10, RED);
    {
        g->cam_y_speed -= 2.f * GetFrameTime();
        g->cam_y += g->cam_y_speed * GetFrameTime();
        BeginMode2D((Camera2D){
            .offset = {.x = 0.f, .y = 0.f}, .target = {.x = 0.f, .y = g->cam_y}, .rotation = 0.f, .zoom = 1.f});
    }
    maze0_draw((Maze0_Cell *)g->maze, GS_FALLFOOD_WIDTH, GS_FALLFOOD_HEIGHT, w);
    {
        Int wid = WINDOW_HEIGHT * 2;
        draw_food_left_general(food_left_to_win, 10, wid * (((Int)(g->cam_y + WINDOW_HEIGHT)) / wid));
    }

    player_draw_extra(g->player, w);
    food_draw(&g->food, w);

    EndDrawing();
    return Level_Return_Continue;
}
