#include "game_state_Seeker.h"
#include "player_related.h"
#include "raylib.h"
#include "very_general.h"
#include <stdlib.h>

void metagame_set_level_Seeker(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_Seeker;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_Seeker;
    mg->data = malloc(sizeof(Game_State_Seeker));
}

void metagame_set_level_Suicide(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_Suicide;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_Suicide;
    mg->data = malloc(sizeof(Game_State_Suicide));
}

void game_state_init_Seeker(Game_State_Seeker *new_g)
{
    Game_State_Seeker g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;
    g.seeker_time_for_move = 1.0;

    g.seeker =
        (Seeker_Snake){.length = 2, .direction = Dir_Left, .positions = {{.x = 26, .y = 10}, {.x = 27, .y = 10}}};
    g.moves = 0;

    *new_g = g;
}

void game_state_init_Suicide(Game_State_Suicide *new_g)
{
    Game_State_Suicide g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 10, .idx_pos = 9, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    set_positions_as_line_from(g.player.positions, g.player.length, (Pos){3, 10}, Dir_Right, &g.w);
    g.time_for_move = 1.0;
    g.seeker_time_for_move = 1.0;

    g.player_flash_timer = 2.0;

    g.seeker =
        (Seeker_Snake){.length = 10, .direction = Dir_Left, .positions = {{.x = 26, .y = 11}, {.x = 27, .y = 10}}};
    set_positions_as_line_from_without_wrapping(g.seeker.positions, g.seeker.length, (Pos){26, 11}, Dir_Right);

    *new_g = g;
}

void seeker_move(Seeker_Snake *snake, World_State0 *w)
{
    for (int i = snake->length - 1; i >= 1; --i)
    {
        snake->positions[i] = snake->positions[i - 1];
    }
    snake->positions[0] = move_inside_grid(snake->positions[0], snake->direction, w);
}

void seeker_draw(const Seeker_Snake *snake, const World_State0 *w)
{
    const Int length = snake->length;
    for (Int j = 0; j < length; ++j)
    {
        draw_block_at(snake->positions[j], (j == 0) ? SKYBLUE : BLUE, w);
    }
}

// returns true on collision
bool seeker_player_collision_logic(const Seeker_Snake *snake, const Player *player)
{
    for (int i = 0; i < player->length; ++i)
    {
        for (int j = 0; j < snake->length; ++j)
        {
            if (pos_equal(player_nth_position(player, i), snake->positions[j]))
            {
                return true;
            }
        }
    }
    return false;
}

Level_Return game_state_frame_Seeker(Game_State_Seeker *g)
{
    World_State0 *w = &g->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    player_set_direction_from_input(&g->player);

    bool seeker_grows = false;

    if (time_move_logic_general(&g->seeker_time_for_move, 0.135894))
    {
        ++g->moves;
        seeker_grows = (g->moves % 25 == 0) && (g->seeker.length < SEEKER_MAX_LENGTH);
        if (seeker_grows)
            g->seeker.length++;

        {
            Pos p_pos = player_nth_position(&g->player, 0);
            Pos s_pos = g->seeker.positions[0];
            Dir d;
            if (abs(p_pos.x - s_pos.x) > abs(p_pos.y - s_pos.y))
            {
                d = ((p_pos.x - s_pos.x) < 0) ? Dir_Left : Dir_Right;
            }
            else
            {
                d = ((p_pos.y - s_pos.y) < 0) ? Dir_Up : Dir_Down;
            }

            Dir prev = g->seeker.direction;
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
            g->seeker.direction = d;
        }

        seeker_move(&g->seeker, w);

        if (seeker_player_collision_logic(&g->seeker, &g->player))
        {
            return Level_Return_Reset_Level;
        }
    }

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        food_player_collision_logic(&g->player, &g->food, w);
    }
    Int food_left_to_win = (DEV ? 8 : 8) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (seeker_grows)
    {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, RED);
    }

    draw_food_left(food_left_to_win);

    player_draw(&g->player, w);
    seeker_draw(&g->seeker, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
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

    bool seeker_grows = false;

    if (time_move_logic_general(&g->seeker_time_for_move, 0.135894))
    {
        if (seeker_grows)
            g->seeker.length++;

        {
            const Pos p_pos = player_nth_position(&g->player, 0);
            const Pos s_pos = g->seeker.positions[0];
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

            Dir prev = g->seeker.direction;
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
            g->seeker.direction = d;
        }

        seeker_move(&g->seeker, w);

        if (seeker_player_collision_logic(&g->seeker, &g->player))
        {
			g->player_flash_timer = GAME_STATE_SUICIDE_FLASH_TIME;
            g->player.length--;

            g->seeker.length = g->player.length;

            Dir_And_Pos stuff = random_outside_edge_position_and_normal(w);
            set_positions_as_line_from_without_wrapping(g->seeker.positions, g->seeker.length, stuff.pos,
                                                        dir_opposite(stuff.dir));
        }
    }

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
    }
    Int food_left_to_win = g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (seeker_grows)
    {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, RED);
    }

    draw_food_left(food_left_to_win);

    if (g->player_flash_timer > 0.f)
    {
        player_draw_extra(&g->player, w);
    }
    else
    {
        player_draw(&g->player, w);
    }

    seeker_draw(&g->seeker, w);

    draw_fps();
    EndDrawing();
    return 0;
}
