#include "game_state1.h"
#include "very_general.h"
#include <stdlib.h>

void metagame_set_level_BlueSnakes(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state1_frame0;
    mg->init_code = (Meta_Game_Init_Code)game_state1_init;
    mg->data = malloc(sizeof(Game_State1));
}

void metagame_set_level_UnSync(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state1_frame_UnSync;
    mg->init_code = (Meta_Game_Init_Code)game_state1_init_UnSync;
    mg->data = malloc(sizeof(Game_State1_UnSync));
}

void game_state1_init(Game_State1 *new_g)
{
    Game_State1 g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;
    g.evil_snake_index = 0;

    *new_g = g;
}

void game_state1_init_UnSync(Game_State1_UnSync *new_g)
{
    Game_State1_UnSync gu;
    Game_State1 g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;
    g.evil_snake_index = 0;

    gu.g1 = g;

    *new_g = gu;
}

Level_Return game_state1_frame0(Game_State1 *g)
{
    World_State0 *w = &g->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        for (Int i = 0; i < g->evil_snake_index; ++i)
        {
            if (seeker_player_collision_logic(&g->snakes[i], &g->player))
            {
                return Level_Return_Reset_Level;
            }
        }

        for (Int i = 0; i < g->evil_snake_index; ++i)
            seeker_move(&g->snakes[i], w);

        food_player_collision_logic(&g->player, &g->food, w);

        // spawn evil snakes
        if (g->evil_snake_index < g->player.length - 1)
        {
            if (g->evil_snake_index < GAME_STATE1_TOTAL_EVIL_SNAKES)
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
    Int food_left_to_win = (DEV ? 8 : 8) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw(&g->player, w);
    for (Int i = 0; i < g->evil_snake_index; ++i)
        seeker_draw(&g->snakes[i], w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}

Level_Return game_state1_frame_UnSync(Game_State1_UnSync *gu)
{
    Game_State1 *g = &gu->g1;
    World_State0 *w = &g->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    player_set_direction_from_input(&g->player);

    for (Int i = 0; i < g->evil_snake_index; ++i)
    {
        if (time_move_logic_general(&gu->evil_snake_time_for_moves[i], gu->evil_snake_intervals[i]))
        {
            if (seeker_player_collision_logic(&g->snakes[i], &g->player))
            {
                return Level_Return_Reset_Level;
            }
            seeker_move(&g->snakes[i], w);
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

        // spawn evil snakes
        if (g->evil_snake_index < g->player.length - 1)
        {
            if (g->evil_snake_index < GAME_STATE1_TOTAL_EVIL_SNAKES)
            {
                // spawn
                g->snakes[g->evil_snake_index] = (Seeker_Snake){.length = 2, Dir_Right, {{0, 5}, {1, 5}}};
                Seeker_Snake *snake = &g->snakes[g->evil_snake_index];
                gu->evil_snake_intervals[g->evil_snake_index] = GetRandomValue(5, 20) / 100.0;
                gu->evil_snake_time_for_moves[g->evil_snake_index] = 0.0;

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
    Int food_left_to_win = (DEV ? 8 : 8) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw(&g->player, w);
    for (Int i = 0; i < g->evil_snake_index; ++i)
        seeker_draw(&g->snakes[i], w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}
