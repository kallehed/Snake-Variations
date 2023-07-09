
#include "game_state_OnceMaze.h"
#include "level_declarations.h"
#include "maze_stuff.h"
#include "player_related.h"
#include "snake_pather.h"
#include "very_general.h"
#include <stdio.h>

void metagame_set_level_OnceMaze(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_OnceMaze_frame;
    mg->init_code = (Meta_Game_Init_Code)game_state_OnceMaze_init;
    mg->size = (sizeof(GS_OnceMaze));
}

void game_state_OnceMaze_init(GS_OnceMaze *new_g)
{
    GS_OnceMaze g;
    g.w = world_state0_init_general(GS_WIDTH, GS_HEIGHT, GS_PIXEL_SIZE);

    g.player = player_init((Pos){.x = GS_WIDTH / 2, .y = 5}, 2, Dir_Right);
    g.points = 0;

    Int snake_lengths[GS_MAX_PATHERS];
    Dir snake_tail_dirs[GS_MAX_PATHERS];
    {
        Int snk_i = 0;
        //      {
        //          const Snake_Pather_Way ways[] = {{Dir_Right, 2}, {Dir_Down, 2}, {Dir_Right, 3}, {Dir_Down, 4},
        //          {Dir_Left, 2}, {Dir_Up, 2}, {Dir_Left, 3}, {Dir_Up, 4}}; g.evil_snake_paths[snk_i] =
        //          snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
        // snake_lengths[snk_i] = 5;
        // snake_tail_dirs[snk_i] = Dir_Down;
        // snk_i++;
        //      }
        {
            Snake_Pather_Way ways[] = {{Dir_Right, 9}, {Dir_Down, 8}, {Dir_Left, 9}, {Dir_Up, 8}};
            snake_lengths[snk_i] = 4;
            snake_tail_dirs[snk_i] = Dir_Down;
            g.evil_snake_paths[snk_i] =
                snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
            snk_i++;
        }
        {
            Snake_Pather_Way ways[] = {{Dir_Right, 3}, {Dir_Down, 2}, {Dir_Left, 3}, {Dir_Up, 2}};
            snake_lengths[snk_i] = 3;
            snake_tail_dirs[snk_i] = Dir_Down;
            g.evil_snake_paths[snk_i] =
                snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
            snk_i++;
        }
        {
            Snake_Pather_Way ways[] = {{Dir_Right, 3}, {Dir_Down, 2}, {Dir_Left, 3}, {Dir_Up, 2}};
            snake_lengths[snk_i] = 3;
            snake_tail_dirs[snk_i] = Dir_Down;
            g.evil_snake_paths[snk_i] =
                snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
            snk_i++;
        }
        {
            Snake_Pather_Way ways[] = {{Dir_Right, 3}, {Dir_Down, 2}, {Dir_Left, 3}, {Dir_Up, 2}};
            snake_lengths[snk_i] = 3;
            snake_tail_dirs[snk_i] = Dir_Down;
            g.evil_snake_paths[snk_i] =
                snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
            snk_i++;
        }
    }

    printf("Size of Maze_Cell type: %lu\n", sizeof(Maze0_Cell));
    printf("Size of matrix: %lu\n", sizeof(g.maze));
    // clang-format off
    const char *maze_str[] = { "----------------------------|-|----------------" ,
                               "---------------------||||||||-|----------------",
                               "-----------||||------|--------|----------------",
                               "-----------|---|||||-|-|||||||-----------------",
                               "-----------|-|-|---|||-||||||||||--------------",
                               "--------||||-|-|-|-|||FP--------|--------------",
                               "-------|-----|-|-|F---|||||||||-|-----||||-----",
                               "------|--|||||F--||||-|||||||||-|-----|F-|-----",
                               "-----|-F||S---------|-|||||||||-|-----|--|-----",
                               "-----|-|||-||||||||-|-|||||||||-|||||||--|-----",
                               "-----|--||-||||||||F--|||||||||----F-----|-----",
                               "------|--|-||||||||-------F--------------|-----",
                               "-------|-|-||||||||-||||||||||||||||||||||-----",
                               "-------|-|F||||||||-|--------------------------",
                               "------|--|-||||||||-|--------------------------",
                               "-----|F-||-||||||||-|--------------------------",
                               "----|--|||----------|--------------------------",
                               "----|-||||||||||||||||||||||||||||||||---------",
                               "----|-S-F--|||---||||||FFFFFFF--------|--------",
                               "----|-------|--|--|||||-FFFF|||||||||--|-------",
                               "----|F--------|||-------||||||||||||||--|------",
                               "----||||||||||||||||||||---|----------|--|-----",
                               "------------||||||||-------|-----------|--|----",
                               "|||||||||||||--F-----------|------------|--||||",
                               "-----------||-|-||||||||||||-------------|-----",
                               "||||||||||-||-|-----F--|------------------|||||",
                               "-----||-------|-||||-|-|--------|||||||--------",
                               "----|---|-|-|-|-----F|-||||||||-|-S---|--------",
                               "----|-|---||--||||||||--------|-|-----|--------",
                               "----|-F|||-|-|--------|||||||-|-|-----|--------", 
                               "----|---------|S-----|------|-|-|||||||--------", 
                               "-----|||||||||-------|------|-|----------------", 
                               "--------------||||||||------|-|----------------", 
                               "----------------------------|-|----------------", 
                               "----------------------------|-|----------------", 
	};
    // clang-format on
    maze0_init_from_string(maze_str, GS_WIDTH, GS_HEIGHT, g.foods, (Maze0_Cell *)g.maze, g.evil_snake_paths,
                           snake_lengths, snake_tail_dirs, NULL, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state_OnceMaze_frame(GS_OnceMaze *g)
{
    World_State0 *w = &g->w;
    // logic

    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.123))
    {
        if (maze0_player_can_move((Maze0_Cell *)g->maze, GS_WIDTH, &g->player, w))
        {
            if (player_move(&g->player, w))
                return Level_Return_Reset_Level;
            g->player.length++;
        }

        for (Int i = 0; i < GS_MAX_PATHERS; ++i)
            snake_pather_move(&g->evil_snake_paths[i], w);

        for (Int i = 0; i < GS_MAX_PATHERS; ++i)
        {
            if (snake_pather_intersect_point(&g->evil_snake_paths[i], player_nth_position(&g->player, 0)))
            {
                return Level_Return_Reset_Level;
            }
        }

        for (Int i = 0; i < GS_FOODS; ++i)
        {
            if (pos_equal(player_nth_position(&g->player, 0), g->foods[i].pos))
            {
                g->points++;
                g->foods[i].pos = (Pos){.x = -1, .y = -1};
            }
        }
    }

    Int food_left_to_win = GS_FOODS - g->points;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    maze0_draw((Maze0_Cell *)g->maze, GS_WIDTH, GS_HEIGHT, w);
    player_draw_extra(&g->player, w);
    for (Int i = 0; i < GS_FOODS; ++i)
    {
        food_draw(&g->foods[i], w);
    }
    for (Int i = 0; i < GS_MAX_PATHERS; ++i)
    {
        snake_pather_draw(&g->evil_snake_paths[i], w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
