
#include "game_state_Seeker.h"

void game_state_Seeker_init(Game_State_Seeker *new_g)
{
    Game_State_Seeker g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;
    g.evil_snake_index = 0;

    *new_g = g;
}

void evil_snake_move(Evil_Snake *snake, World_State0 *w)
{
    for (int i = 0; i < snake->length; ++i)
    {
        snake->positions[i] = move_inside_grid(snake->positions[i], snake->direction, w);
    }
}

void evil_snake_draw(const Evil_Snake *snake, const World_State0 *w)
{
    const Int length = snake->length;
    for (Int j = 0; j < length; ++j)
    {
        draw_block_at(snake->positions[j], (j == 0) ? SKYBLUE : BLUE, w);
    }
}

// returns true on collision
bool evil_snake_player_collision_logic(const Evil_Snake *snake, const Player *player)
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

Level_Return game_state_Seeker_frame(Game_State_Seeker *g)
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
            if (evil_snake_player_collision_logic(&g->evil_snakes[i], &g->player))
            {
                return Level_Return_Reset_Level;
            }
        }

        for (Int i = 0; i < g->evil_snake_index; ++i)
            evil_snake_move(&g->evil_snakes[i], w);

        food_player_collision_logic(&g->player, &g->food, w);

        // spawn evil snakes
        if (g->evil_snake_index < g->player.length - 1)
        {
            if (g->evil_snake_index < GAME_STATE1_TOTAL_EVIL_SNAKES)
            {
                // spawn
                g->evil_snakes[g->evil_snake_index] = (Evil_Snake){.length = 2, Dir_Right, {{0, 5}, {1, 5}}};
                Evil_Snake *snake = &g->evil_snakes[g->evil_snake_index];

                Pos start;
                Pos tail_dir;
                Dir dir;

                switch (GetRandomValue(1, 4))
                {
                case 1: // from left
                    start = (Pos){.x = -1, .y = GetRandomValue(0, w->height - 1)};
                    tail_dir = (Pos){-1, 0};
                    dir = Dir_Right;
                    break;
                case 2: // from right
                    start = (Pos){.x = w->width, .y = GetRandomValue(0, w->height - 1)};
                    tail_dir = (Pos){1, 0};
                    dir = Dir_Left;
                    break;
                case 3: // from top
                    start = (Pos){.x = GetRandomValue(0, w->width - 1), .y = -1};
                    tail_dir = (Pos){0, -1};
                    dir = Dir_Down;
                    break;
                case 4: // from bottom
                    start = (Pos){.x = GetRandomValue(0, w->width - 1), .y = w->height};
                    tail_dir = (Pos){0, 1};
                    dir = Dir_Up;
                    break;
                }
                snake->length = GetRandomValue(2, EVIL_SNAKE_MAX_LENGTH);
                for (Int i = 0; i < snake->length; ++i)
                {
                    snake->positions[i] = (Pos){start.x + tail_dir.x * i, start.y + tail_dir.y * i};
                }
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
        evil_snake_draw(&g->evil_snakes[i], w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}
