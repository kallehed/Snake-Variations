#include "game_state_ObsCourse.h"
#include "player_related.h"
#include "very_general.h"
#include <math.h>

void metagame_set_level_ObsCourse(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_ObsCourse;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_ObsCourse;
    mg->size = (sizeof(Game_State_ObsCourse));
}

void game_state_init_ObsCourse(Game_State_ObsCourse *new_g)
{
    Game_State_ObsCourse g;
    g.w = world_state0_init(20);
    g.w = world_state0_init_general(GS_OBSCOURSE_WIDTH, GS_OBSCOURSE_HEIGHT, WINDOW_HEIGHT / GS_OBSCOURSE_HEIGHT);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = 4, .y = 5};
    food_init_position(&g.food, &g.player, &g.w);
    g.food[0].pos = (Pos){.x = GS_OBSCOURSE_WIDTH - 5, .y = 3};
    g.food[1].pos = (Pos){.x = GS_OBSCOURSE_WIDTH - 6, .y = 6};
    g.food[2].pos = (Pos){.x = GS_OBSCOURSE_WIDTH - 5, .y = 7};
    g.food[3].pos = (Pos){.x = GS_OBSCOURSE_WIDTH - 7, .y = 5};
    g.time_for_move = 1.0;

    g.cam_x = 0.f;

    g.speed_points[0] = (Speed_Point){.from_x = 0, .speed = 1.0f};
    g.speed_points[1] = (Speed_Point){.from_x = 10, .speed = 2.0f};
    g.speed_points[2] = (Speed_Point){.from_x = 20, .speed = 3.0f};
    g.speed_points[3] = (Speed_Point){.from_x = 30, .speed = 4.0f};
    g.speed_points[3] = (Speed_Point){.from_x = 1000, .speed = 10.0f};

    const char *map[] = {
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------|-----------------------------------------------------------------------------------",
        "------------------|---------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
        "----------------------------------------------------------------------------------------------------",
    };

    maze0_init_from_string(map, GS_OBSCOURSE_WIDTH, GS_OBSCOURSE_HEIGHT, NULL, g.maze, NULL, NULL, NULL, NULL, &g.w);
    
    *new_g = g;
}

// normal snake
Level_Return game_state_frame_ObsCourse(Game_State_ObsCourse *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (maze0_player_move((Maze0_Cell *)g->maze, GS_OBSCOURSE_WIDTH, &g->player, w))
            {
                // player died
                TraceLog(LOG_INFO, "%s", "YOU DIED!");

                return Level_Return_Reset_Level;
            }
        for (Int i = 0; i < GS_OBSCOURSE_MAX_FOODS; ++i)
            food_player_collision_logic_food_disappear(&g->player, &g->food[i], w);
    }

    // die by going offscreen
    {
        if ((player_nth_position(&g->player, 0).x + 1) * w->block_pixel_len < g->cam_x)
        {
            return Level_Return_Reset_Level;
        }
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
        Pos p_pos = player_nth_position(&g->player, 0);
        Coord cam_x_coord = g->cam_x / w->block_pixel_len;
        for (Int i = 1; i < GS_OBSCOURSE_MAX_SPEED_POINTS; ++i)
        {
            if (cam_x_coord < g->speed_points[i].from_x)
            {
                Int are = i - 1;
                Int next = i;
                Coord are_coord = g->speed_points[are].from_x;
                Coord next_coord = g->speed_points[next].from_x;
                float speed_are = g->speed_points[are].speed;
                float speed_next = g->speed_points[next].speed;
                float speed =
                    speed_are + (speed_next - speed_are) * (cam_x_coord - are_coord) / (next_coord - are_coord);
                // found the speed point at which we are
                g->cam_x += speed;
                break;
            }
        }
        BeginMode2D(
            (Camera2D){.offset = {.x = 0.f, .y = 0.f}, .target = {.x = g->cam_x, .y = 0.f}, .rotation = 0.f, .zoom = 1.f});
    }
    maze0_draw((Maze0_Cell *)g->maze, GS_OBSCOURSE_WIDTH, GS_OBSCOURSE_HEIGHT, w);
    {
        Int w = WINDOW_WIDTH * 2;
        draw_food_left_general(food_left_to_win, w * (((Int)(g->cam_x + WINDOW_WIDTH)) / w), -40);
    }
    
    player_draw(&g->player, w);
    food_draw(&g->food, w);

    EndDrawing();
    return Level_Return_Continue;
}
