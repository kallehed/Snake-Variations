#include "game_state_Final.h"
#include "level_declarations.h"
#include "player_related.h"
#include "very_general.h"
#include <stdlib.h>

void level_set_Final(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)GS_frame_Final;
    mg->init_code = (Level_Init_Code)GS_init_Final;
    mg->size = (sizeof(GS_Final));
}

void GS_init_Final(GS_Final *new_g, Allo *allo)
{
    GS_Final g;
    g.w = world_state0_init(24);
    g.player = player_init((Pos){10, 10}, 1, 100, Dir_Right, allo);
    food_init_position(&g.food, g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return GS_frame_Final(GS_Final *g)
{
    const World_State0 *w = &g->w;

    player_set_direction_from_input(g->player);
    if (time_move_logic(&g->time_for_move))
    {
		Pos old_pos = player_nth_position(g->player, 0);
        if (player_move(g->player, w))
        {
            return Level_Return_Reset_Level;
        }
		Pos new_pos = player_nth_position(g->player, 0);
		if (abs(old_pos.x - new_pos.x) + abs(old_pos.y - new_pos.y) > 1) {
			return Level_Return_Reset_Level;
		}
        food_player_collision_logic(g->player, &g->food, w);
    }

    Int food_left_to_win = 51 - g->player->length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw_green(g->player, w);
    food_draw_red(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
