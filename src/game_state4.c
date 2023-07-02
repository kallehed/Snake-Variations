#include "game_state4.h"
#include <stdlib.h>

void metagame_set_level_HidingBoxes(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state4_frame0;
    mg->init_code = (Meta_Game_Init_Code)game_state4_init;
    mg->size = (sizeof(Game_State4));
}

void game_state4_init(Game_State4 *new_g)
{
    Game_State4 g;
    g.w = world_state0_init(60);
    g.player = (Player){.length = 5, .idx_pos = 4, .current_direction = Dir_Right, .next_direction = Dir_Right};
    g.player.positions[4] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player.positions[3] = (Pos){.x = g.w.width / 2 - 1, g.w.height / 2};
    g.player.positions[2] = (Pos){.x = g.w.width / 2 - 2, g.w.height / 2};
    g.player.positions[1] = (Pos){.x = g.w.width / 2 - 3, g.w.height / 2};
    g.player.positions[0] = (Pos){.x = g.w.width / 2 - 4, g.w.height / 2};

    food_init_position(&g.food, &g.player, &g.w);

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {4, 4}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};
    g.boxes[2] = (Box){.p = {40, 5}, .w_h = {12, 11}};
    g.boxes[3] = (Box){.p = {44, 19}, .w_h = {7, 6}};
    g.boxes[4] = (Box){.p = {36, 26}, .w_h = {6, 7}};
    g.boxes[5] = (Box){.p = {42, 35}, .w_h = {5, 9}};
    g.boxes[6] = (Box){.p = {50, 36}, .w_h = {6, 4}};
    g.boxes[7] = (Box){.p = {22, 30}, .w_h = {10, 10}};
    g.boxes[8] = (Box){.p = {18, 10}, .w_h = {14, 8}};
    g.boxes[9] = (Box){.p = {1, 10}, .w_h = {8, 14}};
    g.boxes[10] = (Box){.p = {10, 26}, .w_h = {6, 7}};
    g.boxes[11] = (Box){.p = {3, 30}, .w_h = {4, 3}};
    g.boxes[12] = (Box){.p = {2, 35}, .w_h = {3, 4}};
    g.boxes[13] = (Box){.p = {3, 41}, .w_h = {10, 2}};
    g.boxes[14] = (Box){.p = {15, 5}, .w_h = {10, 3}};
    g.boxes[15] = (Box){.p = {35, 5}, .w_h = {2, 10}};
    g.boxes[16] = (Box){.p = {55, 5}, .w_h = {4, 15}};
    g.boxes[17] = (Box){.p = {35, 20}, .w_h = {3, 3}};
    g.boxes[18] = (Box){.p = {14, 20}, .w_h = {6, 4}};
    g.boxes[19] = (Box){.p = {7, 3}, .w_h = {1, 2}};

    g.time_for_move = 1.0;

    *new_g = g;
}

// Boxes hiding food
Level_Return game_state4_frame0(Game_State4 *g)
{

    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.08))
    {
        if (player_move(&g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        boxes_collision_logic(g->boxes, GAME_STATE4_BOXES, player_nth_position(&g->player, 0),
                                     g->player.current_direction, (Pos){1, 1}, -1, &g->w);

        food_player_collision_logic(&g->player, &g->food, &g->w);
    }
    Int food_left_to_win = (DEV ? 16 : 16) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw_extra(&g->player, &g->w);
    food_draw(&g->food, &g->w);
    for (Int i = 0; i < GAME_STATE4_BOXES; i++)
    {
        box_draw(&g->boxes[i], &g->w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
