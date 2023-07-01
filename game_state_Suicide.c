#include "game_state_Suicide.h"
#include <math.h>
#include <stdlib.h>

void metagame_set_level_Suicide(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_Suicide;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_Suicide;
    mg->data = malloc(sizeof(Game_State_Suicide));
}

void game_state_init_Suicide(Game_State_Suicide *new_g)
{
    Game_State_Suicide g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 10, .idx_pos = 9, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    set_positions_as_line_from(g.player.positions, g.player.length, (Pos){3, 10}, Dir_Right, &g.w);
    g.time_for_move = 1.0;

    g.player_flash_timer = -1.f;

    for (Int i = 0; i < GAME_STATE_SUICIDE_MAX_SEEKERS; ++i)
    {
        g.seeker_time_for_moves[i] = 1.0;
        g.seeker_wait_times[i] = 0.13273 + GetRandomValue(0, 100) / 100.0;
        g.seekers[i] = (Seeker_Snake){.length = GetRandomValue(6, 10), .direction = Dir_Left};
        set_positions_as_line_from_without_wrapping(g.seekers[i].positions, g.seekers[i].length, (Pos){26, 2 + i * 3},
                                                    Dir_Right);
    }

    *new_g = g;
}

Level_Return game_state_frame_Suicide(Game_State_Suicide *g)
{
    World_State0 *w = &g->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    if (g->player_flash_timer > 0.f)
    {
        g->player_flash_timer -= GetFrameTime();
    }

    player_set_direction_from_input(&g->player);

    // For seeker
    for (Int i = 0; i < GAME_STATE_SUICIDE_MAX_SEEKERS; ++i)
    {
        if (time_move_logic_general(&g->seeker_time_for_moves[i], g->seeker_wait_times[i]))
        {
            g->seeker_wait_times[i] = 0.05 + fabs(0.2 * sin(0.5 * GetTime() + g->seeker_time_for_moves[i]));

            if (GetRandomValue(1, 10) == 1)
            {
                g->seekers[i].direction = GetRandomValue(0, 3);
            }
            else
            {
                const Pos p_pos = player_nth_position(&g->player, 0);
                const Pos s_pos = g->seekers[i].positions[0];
                Dir d;
                { // compute the dir which is closest to player, so you can avoid the player
                    Int left_dist = s_pos.x - p_pos.x;
                    if (left_dist < 0)
                        left_dist += GAME_STATE_SUICIDE_WIDTH;
                    Int right_dist = p_pos.x - s_pos.x;
                    if (right_dist < 0)
                        right_dist += GAME_STATE_SUICIDE_WIDTH;
                    Int up_dist = s_pos.y - p_pos.y;
                    if (up_dist < 0)
                        up_dist += GAME_STATE_SUICIDE_HEIGHT;
                    Int down_dist = s_pos.y - p_pos.y;
                    if (down_dist < 0)
                        down_dist += GAME_STATE_SUICIDE_HEIGHT;

                    const Int nums[] = {right_dist, left_dist, up_dist, down_dist};
                    const Int i = smallest_index_of_numbers(nums, 4);
                    // go in opposite direction to what dir is closest to player
                    const Dir dirs[] = {Dir_Left, Dir_Right, Dir_Down, Dir_Up};
                    d = dirs[i];
                }

                Dir prev = g->seekers[i].direction;
                if (prev == Dir_Right && d == Dir_Left)
                {
                    d = Dir_Up;
                }
                else if (prev == Dir_Left && d == Dir_Right)
                {
                    d = Dir_Up;
                }
                else if (prev == Dir_Up && d == Dir_Down)
                {
                    d = Dir_Left;
                }
                else if (prev == Dir_Down && d == Dir_Up)
                {
                    d = Dir_Left;
                }
                g->seekers[i].direction = d;
            }

            seeker_move(&g->seekers[i], w);
        }
    }

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        if (g->player_flash_timer <= 0.f)
        {
            for (Int i = 0; i < GAME_STATE_SUICIDE_MAX_SEEKERS; ++i)
            {
                if (seeker_player_collision_logic(&g->seekers[i], &g->player))
                {
                    g->player_flash_timer = GAME_STATE_SUICIDE_FLASH_TIME;
                    g->player.length--;

                    g->seekers[i].length = g->player.length;

                    Dir_And_Pos stuff = random_outside_edge_position_and_normal(w);
                    set_positions_as_line_from_without_wrapping(g->seekers[i].positions, g->seekers[i].length,
                                                                stuff.pos, dir_opposite(stuff.dir));
                    break;
                }
            }
        }
    }
    Int food_left_to_win = g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    Color clear_color = (GAME_STATE_SUICIDE_FLASH_TIME - g->player_flash_timer < 0.2f) ? BLACK : RAYWHITE;
    ClearBackground(clear_color);

    draw_food_left(food_left_to_win);

    if (g->player_flash_timer > 0.f)
    {
        player_draw_flashing(&g->player, w);
    }
    else
    {
        player_draw(&g->player, w);
    }

    {
        const Color colors_for_head[] = {SKYBLUE, BLACK, PINK, GREEN};
        const Color colors_for_body[] = {BLUE, WHITE, VIOLET, LIME};
        for (Int i = 0; i < GAME_STATE_SUICIDE_MAX_SEEKERS; ++i)
        {
            seeker_draw_general(&g->seekers[i], colors_for_head[i], colors_for_body[i], w);
        }
    }

    draw_fps();
    EndDrawing();
    return 0;
}
