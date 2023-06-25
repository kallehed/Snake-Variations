#include "game_state_maze.h"
#include <stdio.h>

void game_state_Maze_init(Game_State_Maze *new_g)
{
    Game_State_Maze g;
    g.w = world_state0_init(GAME_STATE_MAZE_WIDTH);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};

    g.evil_snake_paths[0] = (Snake_Pather){
        .len = 4,
        .positions = {{29, 3}, {29, 4}, {29, 5}, {29, 6}},
        .ways = {{Dir_Right, 2}, {Dir_Down, 6}, {Dir_Left, 2}, {Dir_Up, 6}},
        .ways_len = 4,
        .way_idx = 4, // be at end
        .walk_this_way_counter = 0,
    };
    g.evil_snake_paths[1] = (Snake_Pather){
        .len = 4,
        .positions = {{12, 10}, {12, 11}, {12, 12}, {12, 13}},
        .ways = {{Dir_Right, 14}, {Dir_Down, 7}, {Dir_Left, 14}, {Dir_Up, 7}},
        .ways_len = 4,
        .way_idx = 4, // be at end
        .walk_this_way_counter = 0,
    };

    printf("Size of Maze_Cell type: %lu\n", sizeof(Maze_Cell)); // this is SO stupid, why is the enum 4 bytes??!?!?!
    printf("Size of matrix: %lu\n", sizeof(g.maze));
    const char *const maze_str = "----------------------------------------" // S does not do anything, just marker
                                 "----------------------------------------"
                                 "-|||||||------------------||||||||||||--"
                                 "-|-----|------------------|--S-------|--"
                                 "-|-----|------------------|----------|--"
                                 "-|--F--|------------------|--------F-|--"
                                 "-|-----|------------------|----------|--"
                                 "-|-----|------------------|----------|--"
                                 "-|||--||----------|||||||||----------|--"
                                 "---|--|----||||||||------------------|--"
                                 "---|--|----|S---------------||||||||||--"
                                 "---|--|----|-||||||-||||||--|--------|--"
                                 "---|--||||||-|-----------|-||--------|--"
                                 "---|---------|-----------|-|-----F---|--"
                                 "---|||||||||-|-----------|-|---------|--"
                                 "-----------|-|-----------|-|---------|--"
                                 "-----------|-|||||||||||||-|--||||||||--"
                                 "-----------|---------------|-|-------|--"
                                 "---------|||||||||||-|||||||-|--|-|--|--"
                                 "--||||||-|---|||||||-|-------|--|-||-|--"
                                 "--|----|-|-|-|||||||-|----|||||||-|--|--"
                                 "--|-F--|-|-|-|---|||-|------------|-||--"
                                 "--|----|-|-|-|-|-|||-|----||||-||||--|--"
                                 "--|----|-|-|-|-|-|||-|---|---|-|---|-|--"
                                 "--|----|-|-|-|-|-|||-|--||---|||-----|--"
                                 "--|----|||-|-|-|-|||-|-|||---|-------|--"
                                 "--|--------|---|---------------------|--"
                                 "--||||||||||||||||||||||||||||||||||||--"
                                 "----------------------------------------"
                                 "----------------------------------------";

    Int food_idx = 0;
    for (Int i = 0; i < GAME_STATE_MAZE_HEIGHT; ++i)
    {
        for (Int j = 0; j < GAME_STATE_MAZE_WIDTH; ++j)
        {
            Int idx = i * GAME_STATE_MAZE_WIDTH + j;
            Maze_Cell cell = Maze_Cell_Empty;
            switch (maze_str[idx])
            {
            case '|': {
                cell = Maze_Cell_Wall;
            }
            break;
            case 'F': {
                g.foods[food_idx++] = (Food){.pos = {.x = j, .y = i}};
            }
            break;
            default: {
                cell = Maze_Cell_Empty;
            }
            break;
            }
            g.maze[i][j] = cell;
        }
    }
    g.time_for_move = 1.0;

    *new_g = g;
}

Level_Return game_state_Maze_frame(Game_State_Maze *g)
{
    World_State0 *w = &g->w;
    // logic

    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.123))
    {
        Pos next_pos = move_inside_grid(player_nth_position(&g->player, 0), g->player.next_direction, w);
        if (g->maze[next_pos.y][next_pos.x] != Maze_Cell_Wall) // check that the next position is not in a wall
        {
            if (player_move(&g->player, w))
            {
                // player died
                TraceLog(LOG_INFO, "%s", "YOU DIED!");

                return Level_Return_Reset_Level;
            }
        }
        for (Int i = 0; i < GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS; ++i)
            snake_pather_move(&g->evil_snake_paths[i], w);

        for (Int i = 0; i < GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS; ++i)
            if (snake_pather_player_intersection(&g->evil_snake_paths[i], &g->player))
                return Level_Return_Reset_Level;

        for (Int i = 0; i < GAME_STATE_MAZE_FOODS; ++i)
        {
            if (pos_equal(player_nth_position(&g->player, 0), g->foods[i].pos))
            {
                ++g->player.length;
                g->foods[i].pos = (Pos){.x = -1, .y = -1};
            }
        }
    }

    Level_Return food_left_to_win = (DEV ? 5 : 5) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    maze_draw((Maze_Cell *)g->maze, GAME_STATE_MAZE_WIDTH, GAME_STATE_MAZE_HEIGHT, w);
    player_draw_extra(&g->player, w);
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


void maze_draw(const Maze_Cell maze[], const Int maze_width, const Int maze_height, const World_State0 *w)
{
    for (Int i = 0; i < maze_height; ++i)
    {
        for (Int j = 0; j < maze_width; ++j)
        {
            Maze_Cell cell = maze[i * maze_width + j];
            Color col;
            switch (cell)
            {
            case Maze_Cell_Empty: {
                continue;
            }
            break;
            case Maze_Cell_Wall: {
                col = (Color){0, 0, 0, 200};
            }
            break;
            }
            draw_block_at((Pos){j, i}, col, w);
        }
    }
}


void snake_pather_draw(Snake_Pather *snake_pather, World_State0 *w)
{
    for (Int i = 0; i < snake_pather->len; ++i)
    {
        draw_block_at(snake_pather->positions[i], (i == 0) ? SKYBLUE : BLUE, w);
    }
}

void snake_pather_move(Snake_Pather *snake_pather, World_State0 *w)
{
    // get new way
    // printf("path move counter: %d\n", snake_pather->walk_this_way_counter);
    if (0 >= snake_pather->walk_this_way_counter)
    {
        snake_pather->way_idx++;
        // we have gone all the ways, return to first again
        if (snake_pather->way_idx >= snake_pather->ways_len)
        {
            snake_pather->way_idx = 0;
        }
        snake_pather->walk_this_way_counter = snake_pather->ways[snake_pather->way_idx].len;
    }

    // evil_snake_move(&snake_path->snake, w);
    Dir dir = snake_pather->ways[snake_pather->way_idx].dir;
    for (Int i = snake_pather->len - 1; i >= 1; --i)
    {
        snake_pather->positions[i] = snake_pather->positions[i - 1];
    }
    snake_pather->positions[0] = move_inside_grid(snake_pather->positions[0], dir, w);
    --snake_pather->walk_this_way_counter;
}

bool snake_pather_player_intersection(Snake_Pather *snake_pather, Player *player)
{
    for (Int i = 0; i < player->length; ++i)
    {
        for (Int j = 0; j < snake_pather->len; ++j)
        {
            if (pos_equal(snake_pather->positions[j], player_nth_position(player, i)))
            {
                return true;
            }
        }
    }
    return false;
}
