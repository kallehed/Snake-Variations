#include "game_state_StaticPlatformer.h"
#include "very_general.h"

void game_state_init_StaticPlatformer(Game_State_StaticPlatformer *new_g)
{
    Game_State_StaticPlatformer g;
    g.w = world_state0_init(30);
	// HEIGHT 22, WIDTH 30
    g.player = (Player){.idx_pos = 1,
                        .length = 2,
                        .next_direction = Dir_Right,
                        .current_direction = Dir_Nothing,
                        .positions = {{.x = 3, .y = 3}, {.x = 2, .y = 20}}};
	*new_g = g;
}

Level_Return game_state_frame_StaticPlatformer(Game_State_StaticPlatformer *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.2))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = (DEV ? 13 : 12) - g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
