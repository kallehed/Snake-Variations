#include "game_state_maze.h"
#include "level_declarations.h"
#include "maze_stuff.h"
#include "snake_pather.h"
#include <stdio.h>
#include <stdlib.h>

void level_set_Maze(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_Maze_frame;
    mg->init_code = (Level_Init_Code)game_state_init_Maze;
    mg->size = (sizeof(Game_State_Maze));
}

void game_state_init_Maze(Game_State_Maze *new_g, Allo *allo, Sound sounds[])
{
    Game_State_Maze g;
    g.w = world_state0_init(GAME_STATE_MAZE_WIDTH, sounds);
    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 2, 100, Dir_Right, allo);

    Snake_Pather_Way ways0[] = {{Dir_Right, 2}, {Dir_Down, 6}, {Dir_Left, 2}, {Dir_Up, 6}};
    g.evil_snake_paths[0] = snake_pather_init_except_position(ways0, 4);

    Snake_Pather_Way ways1[] = {{Dir_Right, 14}, {Dir_Down, 7}, {Dir_Left, 14}, {Dir_Up, 7}};
    g.evil_snake_paths[1] = snake_pather_init_except_position(ways1, 4);

    printf("Size of Maze_Cell type: %lu\n",
           sizeof(Maze0_Cell)); // this WAS SO stupid, why are enums minimum 4 bytes??!?!?!
    printf("Size of matrix: %lu\n", sizeof(g.maze));
    // clang-format off
    const char *maze_str[] = { "----------------------------------------" ,
                               "----------------------------------------",
                               "-|||||||------------------||||||||||||--",
                               "-|-----|------------------|--S-------|--",
                               "-|-----|------------------|----------|--",
                               "-|--F--|------------------|--------F-|--",
                               "-|-----|------------------|----------|--",
                               "-|-----|------------------|----------|--",
                               "-|||--||----------|||||||||----------|--",
                               "---|--|----||||||||------------------|--",
                               "---|--|----|S---------------||||||||||--",
                               "---|--|----|-||||||-||||||--|--------|--",
                               "---|--||||||-|-----------|-||--------|--",
                               "---|---------|-----------|-|-----F---|--",
                               "---|||||||||-|-----------|-|---------|--",
                               "-----------|-|-----------|-|---------|--",
                               "-----------|-|||||||||||||-|--||||||||--",
                               "-----------|---------------|-|-------|--",
                               "---------|||||||||||-|||||||-|--|-|--|--",
                               "--||||||-|---|||||||-|-------|--|-||-|--",
                               "--|----|-|-|-|||||||-|----|||||||-|--|--",
                               "--|-F--|-|-|-|---|||-|------------|-||--",
                               "--|----|-|-|-|-|-|||-|----||||-||||--|--",
                               "--|----|-|-|-|-|-|||-|---|---|-|---|-|--",
                               "--|----|-|-|-|-|-|||-|--||---|||-----|--",
                               "--|----|||-|-|-|-|||-|-|||---|-------|--",
                               "--|--------|---|---------------------|--",
                               "--||||||||||||||||||||||||||||||||||||--",
                               "----------------------------------------",
                               "----------------------------------------", };
	//clang-format on
	Int lengths[] = {4, 4};
	Dir dirs[] = {Dir_Down, Dir_Down};
    maze0_init_from_string(maze_str, GAME_STATE_MAZE_WIDTH, GAME_STATE_MAZE_HEIGHT, g.foods, (Maze0_Cell *)g.maze, g.evil_snake_paths, lengths, dirs, NULL, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state_Maze_frame(Game_State_Maze *g)
{
    World_State0 *w = &g->w;
    // logic

    player_set_direction_from_input(g->player);

    if (time_move_logic_general(&g->time_for_move, 0.123))
    {
		maze0_player_move((Maze0_Cell *)g->maze, GAME_STATE_MAZE_WIDTH, g->player, w);

        for (Int i = 0; i < GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS; ++i)
            snake_pather_move(&g->evil_snake_paths[i], w);

        for (Int i = 0; i < GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS; ++i)
        {
            if (snake_pather_player_intersection(&g->evil_snake_paths[i], g->player))
            {
                return Level_Return_Reset_Level;
            }
        }

        for (Int i = 0; i < GAME_STATE_MAZE_FOODS; ++i)
        {
            food_player_collision_logic_food_disappear(g->player, &g->foods[i], w);
        }
    }

    Int food_left_to_win = 6 - g->player->length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    maze0_draw((Maze0_Cell *)g->maze, GAME_STATE_MAZE_WIDTH, GAME_STATE_MAZE_HEIGHT, w);
    player_draw_extra(g->player, w);
    for (Int i = 0; i < GAME_STATE_MAZE_FOODS; ++i)
    {
        food_draw(&g->foods[i], w);
    }
    for (Int i = 0; i < GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS; ++i)
    {
        snake_pather_draw(&g->evil_snake_paths[i], w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
