#include "game_state_Attack.h"
#include "raylib.h"
#include "very_general.h"
#include <stdlib.h>

void metagame_set_level_Attack(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_Attack;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_Attack;
    mg->data = malloc(sizeof(Game_State_Attack));
}

void game_state_init_Attack(Game_State_Attack *new_g)
{
    Game_State_Attack g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 12, .idx_pos = 11, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    set_positions_as_line_from_without_wrapping(g.player.positions, g.player.length,
                                                (Pos){.x = -5 + g.w.width / 2, g.w.height / 2}, Dir_Right);

    g.player_inv_timer = 0.f;
    g.time_for_move = 1.0;
    g.start_time = GetTime();
    g.frames_gone_without_spawn = 0;
    g.evil_snake_index = 0;

    *new_g = g;
}

Level_Return game_state_frame_Attack(Game_State_Attack *g)
{
    World_State0 *w = &g->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    if (g->player_inv_timer > 0.f)
    {
        g->player_inv_timer -= GetFrameTime();
    }

    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }

        if (g->player_inv_timer <= 0.f)
        {
            for (Int i = 0; i < g->evil_snake_index; ++i)
            {
                if (seeker_player_collision_logic(&g->snakes[i], &g->player))
                {
                    g->player.length -= 1;
					g->player_inv_timer = GS_PLAYER_INV_TIME;
					set_positions_as_line_from_without_wrapping(g->snakes[i].positions,g->snakes[i].length,(Pos){.x=-1,.y=-1},Dir_Nothing);
					g->snakes[i].direction = Dir_Nothing;
                    break;
                }
            }
        }

        if (g->player.length <= 0)
        {
            return Level_Return_Reset_Level;
        }

        for (Int i = 0; i < g->evil_snake_index; ++i)
            seeker_move(&g->snakes[i], w);

        // spawn evil snakes
        g->frames_gone_without_spawn++;

        if (GetRandomValue(1, 30 - g->frames_gone_without_spawn) == 1)
        {
			g->frames_gone_without_spawn = 0;
            if (g->evil_snake_index < GS_MAX_SNAKES)
            {
                // spawn
                g->snakes[g->evil_snake_index] = (Seeker_Snake){.length = 2, Dir_Right, {{0, 5}, {1, 5}}};
                Seeker_Snake *snake = &g->snakes[g->evil_snake_index];

                const Dir_And_Pos stuff = random_outside_edge_position_and_normal(w);
                Pos start = stuff.pos;
                Dir dir = stuff.dir;
                Dir tail_dir = dir_opposite(dir);

                snake->length = GetRandomValue(2, 10);
                set_positions_as_line_from_without_wrapping(snake->positions, snake->length, start, tail_dir);
                snake->direction = dir;
                ++g->evil_snake_index;
            }
        }
    }
    Int food_left_to_win = GS_TIME_TO_WIN - (Int)(GetTime() - g->start_time);

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    Color clear_color = (GS_PLAYER_INV_TIME -  g->player_inv_timer < 0.2f) ? BLACK : RAYWHITE;
    ClearBackground(clear_color);

    draw_food_left(food_left_to_win);

    if (g->player_inv_timer > 0.f)
    {
        player_draw_flashing(&g->player, w);
    }
    else
    {
        player_draw(&g->player, w);
    }

    for (Int i = 0; i < g->evil_snake_index; ++i)
        seeker_draw(&g->snakes[i], w);

    draw_fps();
    EndDrawing();
    return 0;
}
