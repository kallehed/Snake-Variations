#include "game_state_YouFood.h"
#include <stdlib.h>

void metagame_set_level_YouFood(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_YouFood_frame;
    mg->init_code = (Meta_Game_Init_Code)game_state_YouFood_init;
    mg->data = malloc(sizeof(Game_State_YouFood));
}

void game_state_YouFood_init(Game_State_YouFood *new_g)
{
    Game_State_YouFood g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.food_dir = Dir_Nothing;
    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state_YouFood_frame(Game_State_YouFood *g)
{
    World_State0 *w = &g->w;
    // logic

    {
        Dir new_dir = get_dir_from_input();
        if (new_dir != Dir_Nothing)
            g->food_dir = new_dir;
    }

    if (time_move_logic_general(&g->time_for_move, 0.135))
    {
        if (GetRandomValue(1, 3) == 1)
            player_set_direction_correctly(&g->player, GetRandomValue(0, 3));
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            // return Level_Return_Reset_Level;
        }
        g->food.pos = move_inside_grid(g->food.pos, g->food_dir, w);
        g->food_dir = Dir_Nothing;
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Level_Return food_left_to_win = (DEV ? 7 : 7) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}