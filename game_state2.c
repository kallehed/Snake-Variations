#include "game_state2.h"
void game_state2_init(Game_State2 *new_g)
{
    Game_State2 g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 5, .idx_pos = 4, .current_direction = Dir_Right, .next_direction = Dir_Right};
    g.player.positions[4] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player.positions[3] = (Pos){.x = g.w.width / 2 - 1, g.w.height / 2};
    g.player.positions[2] = (Pos){.x = g.w.width / 2 - 2, g.w.height / 2};
    g.player.positions[1] = (Pos){.x = g.w.width / 2 - 3, g.w.height / 2};
    g.player.positions[0] = (Pos){.x = g.w.width / 2 - 4, g.w.height / 2};

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {1, 1}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};

    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state2_frame0(Game_State2 *g)
{
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        boxes_player_collision_logic(g->boxes, GAME_STATE2_BOXES, player_nth_position(&g->player, 0),
                                     g->player.current_direction, (Pos){1, 1}, -1, &g->w);
    }

    Pos pos1 = {20, 15};
    Pos pos2 = {5, 5};

    Int points = 0;

    for (Int i = 0; i < GAME_STATE2_BOXES; ++i)
    {
        if (pos_equal(pos1, g->boxes[i].p) || pos_equal(pos2, g->boxes[i].p))
        {
            ++points;
        }
    }

    if (points == 2)
    {
        return Level_Return_Next_Level;
    }

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(2 - points);
    player_draw_extra(&g->player, &g->w);
    draw_block_at(pos1, GREEN, &g->w);
    draw_block_at(pos2, GREEN, &g->w);
    for (Int i = 0; i < GAME_STATE2_BOXES; i++)
    {
        box_draw(&g->boxes[i], &g->w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
