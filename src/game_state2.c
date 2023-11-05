#include "game_state2.h"
#include "player_related.h"
#include <stdlib.h>
#include "level_declarations.h"
void level_set_Boxes(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state2_frame0;
    mg->init_code = (Level_Init_Code)game_state2_init;
    mg->size = (sizeof(Game_State2));
}
void game_state2_init(Game_State2 *new_g, Allo *allo, Sound sounds[])
{
    Game_State2 g;
    g.w = world_state0_init(24, sounds);

    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 5, 100, Dir_Right, allo);

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {1, 1}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};

    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state2_frame0(Game_State2 *g)
{
    // logic
    player_set_direction_from_input(g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        boxes_collision_logic(g->boxes, GAME_STATE2_BOXES, player_nth_position(g->player, 0),
                                     g->player->current_direction, (Pos){1, 1}, -1, &g->w);
    }

    Pos pos[GAME_STATE2_MAX_POS] = {
        {20, 15},
        {5, 5},
        {2, 13},
    };

    Int points = 0;

    for (Int i = 0; i < GAME_STATE2_BOXES; ++i)
    {
        for (Int pos_idx = 0; pos_idx < GAME_STATE2_MAX_POS; ++pos_idx)
        {
            if (pos_equal(pos[pos_idx], g->boxes[i].p))
            {
                ++points;
            }
        }
    }
    points += player_intersection_points(g->player, pos, GAME_STATE2_MAX_POS);

    if (points == GAME_STATE2_MAX_POS)
    {
        return Level_Return_Next_Level;
    }

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(3 - points);
    for (Int i = 0; i < GAME_STATE2_MAX_POS; ++i)
    {
        draw_block_at(pos[i], GREEN, &g->w);
    }
    player_draw_extra(g->player, &g->w);
    for (Int i = 0; i < GAME_STATE2_BOXES; i++)
    {
        box_draw(&g->boxes[i], &g->w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
