#include "game_state_YouBlue.h"
#include "level_declarations.h"
#include "player_related.h"
#include "very_general.h"

void level_set_YouBlue(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)gs_frame_YouBlue;
    mg->init_code = (Level_Init_Code)gs_init_YouBlue;
    mg->size = (sizeof(GS_YouBlue));
}

void gs_init_YouBlue(GS_YouBlue *new_g, Allo *allo, Sound sounds[])
{
    GS_YouBlue g;
    g.w = world_state0_init(28, sounds);
    g.start_time = GetTime();

    for (Int i = 0; i < GS_YOUBLUE_TOTAL_PLAYERS; ++i)
    {
        g.players[i] = player_init((Pos){g.w.width / 2, g.w.height / 2}, 8, 100, Dir_Right, allo);
    }

    g.player_inv_timers[0] = 0.f;
    g.player_index = 1;
    g.turn_dir = Dir_Nothing;
    g.time_for_move = 1.0;
    g.seeker_index = 0;

    *new_g = g;
}

Level_Return gs_frame_YouBlue(GS_YouBlue *g)
{
    World_State0 *w = &g->w;
    // logic

    for (Int p = 0; p < g->player_index; ++p)
    {

        if (g->player_inv_timers[p] > 0.f)
        {
            g->player_inv_timers[p] -= GetFrameTime();
        }
    }

    {
        Dir new_dir = get_dir_from_input();
        if (new_dir != Dir_Nothing)
        {
            g->turn_dir = new_dir;
        }
    }

    if (time_move_logic(&g->time_for_move))
    {
        for (Int p = 0; p < g->player_index; ++p)
        {
            if (GetRandomValue(1, 10) == 1)
            {
                player_set_direction_correctly(g->players[p], GetRandomValue(0, 3));
            }
            player_move(g->players[p], w);
        }
        for (Int p = 0; p < g->player_index; ++p)
        {
            if (g->player_inv_timers[p] <= 0.f)
            {
                for (Int i = 0; i < g->seeker_index; ++i)
                {
                    if (seeker_player_collision_logic(&g->snakes[i], g->players[p]))
                    {
                        g->player_inv_timers[p] = GS_YOUBLUE_PLAYER_INV_TIME;
                        g->players[p]->length--;
                        break;
                    }
                }
            }
        }

        for (Int i = 0; i < g->seeker_index; ++i)
        {
            if (g->turn_dir == Dir_Right)
            {
                g->snakes[i].direction = dir_turn_clockwise(g->snakes[i].direction);
            }
            else if (g->turn_dir == Dir_Left)
            {
                g->snakes[i].direction = dir_turn_counter_clockwise(g->snakes[i].direction);
            }
            seeker_move(&g->snakes[i], w);
        }
        g->turn_dir = Dir_Nothing;

        // spawn evil snakes
        if (GetTime() - 4.0 * g->seeker_index > g->start_time)
        {
            if (g->seeker_index < GS_YOUBLUE_TOTAL_SEEKERS)
            {
                // spawn
                Seeker_Snake *snake = &g->snakes[g->seeker_index];

                const Dir_And_Pos stuff = random_outside_edge_position_and_normal(w);
                Pos start = stuff.pos;
                Dir dir = stuff.dir;
                Dir tail_dir = dir_opposite(dir);

                snake->length = GetRandomValue(2, 3);
                set_positions_as_line_from_without_wrapping(snake->positions, snake->length, start, tail_dir);
                snake->direction = dir;
                ++g->seeker_index;
            }
        }
        // spawn player
        if (GetTime() - 7.0 * g->player_index > g->start_time)
        {
            if (g->player_index < GS_YOUBLUE_TOTAL_PLAYERS)
            {
                player_set_positions(g->players[g->player_index], (Pos){0, 0}, 6, Dir_Right);
                ++g->player_index;
            }
        }
    }
    Int food_left_to_win = 0;
    for (Int p = 0; p < g->player_index; ++p)
    {
        food_left_to_win += g->players[p]->length;
    }

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    {
        Color col = RAYWHITE;
        for (Int p = 0; p < g->player_index; ++p)
        {
            if (GS_YOUBLUE_PLAYER_INV_TIME - g->player_inv_timers[p] < 0.07f)
            {
                col = BLACK;
                break;
            }
        }

        ClearBackground(col);
    }

    draw_food_left(food_left_to_win);

    for (Int p = 0; p < g->player_index; ++p)
    {
        if (g->player_inv_timers[p] <= 0.f)
        {
            player_draw(g->players[p], w);
        }
        else
        {
            player_draw_flashing(g->players[p], w);
        }
    }

    for (Int i = 0; i < g->seeker_index; ++i)
        seeker_draw(&g->snakes[i], w);

    draw_fps();
    EndDrawing();
    return 0;
}
