#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GAME_FPS 144

#define HEIGHT 25
#define WIDTH 30
#define BLOCK_PIXEL_LEN 25

#define START_X (WIDTH / 2)
#define START_Y (HEIGHT / 2)

typedef int8_t Coord; // Coord in world

typedef struct Pos
{
    Coord x, y;
} Pos;

typedef enum Dir
{
    Dir_Right = 0,
    Dir_Left,
    Dir_Up,
    Dir_Down,
    Dir_Nothing,
} Dir;

#define PLAYER_MAX_POSITIONS (HEIGHT * WIDTH)
typedef struct Player
{
    int32_t idx_pos;
    int32_t length;
    Dir next_direction;
    Dir current_direction;
    Pos positions[PLAYER_MAX_POSITIONS];
} Player;

typedef struct Food
{
    Pos pos;
} Food;

#define EVIL_SNAKE_MAX_LENGTH 10
typedef struct Evil_Snake
{
    int32_t length;
    Dir direction;
    Pos positions[EVIL_SNAKE_MAX_LENGTH];
} Evil_Snake;

static bool pos_equal(Pos p, Pos q)
{
    return p.x == q.x && p.y == q.y;
}

static void draw_block_at(Pos pos, Color color)
{
    DrawRectangle(pos.x * BLOCK_PIXEL_LEN, pos.y * BLOCK_PIXEL_LEN, BLOCK_PIXEL_LEN, BLOCK_PIXEL_LEN, color);
}

// Cycle through positions of player
static void player_draw(const Player *player)
{
    int32_t drawn_cells = 0;
    for (int32_t i = player->idx_pos; drawn_cells < player->length; ++drawn_cells)
    {
        const Pos *pos = &player->positions[i];
        draw_block_at(*pos, RED);

        --i;
        if (i < 0)
        {
            i = PLAYER_MAX_POSITIONS - 1;
        }
    }
}

static Pos player_nth_position(const Player *player, int32_t idx)
{
    int32_t i = player->idx_pos - idx;
    while (i < 0)
        i += PLAYER_MAX_POSITIONS;
    return player->positions[i];
}

static void player_set_direction(Player *player)
{
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_L))
    {
        if (player->current_direction != Dir_Left)
            player->next_direction = Dir_Right;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_H))
    {
        if (player->current_direction != Dir_Right)
            player->next_direction = Dir_Left;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_K))
    {
        if (player->current_direction != Dir_Down)
            player->next_direction = Dir_Up;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_J))
    {
        if (player->current_direction != Dir_Up)
            player->next_direction = Dir_Down;
    }
}

static Pos move_inside_grid(Pos pos, const Dir dir)
{
    switch (dir)
    {
    case Dir_Right:
        ++pos.x;
        if (pos.x >= WIDTH)
            pos.x = 0;
        break;
    case Dir_Left:
        --pos.x;
        if (pos.x < 0)
            pos.x = WIDTH - 1;
        break;
    case Dir_Up:
        --pos.y;
        if (pos.y < 0)
            pos.y = HEIGHT - 1;
        break;
    case Dir_Down:
        ++pos.y;
        if (pos.y >= HEIGHT)
            pos.y = 0;
        break;
    case Dir_Nothing:
        break;
    }
    return pos;
}

// returns true if player dies
static bool player_move(Player *player)
{
    Pos prev_pos = player->positions[player->idx_pos];

    // change prev pos to new position
    player->current_direction = player->next_direction;
    if (player->current_direction == Dir_Nothing)
        return false;
    prev_pos = move_inside_grid(prev_pos, player->current_direction);

    ++player->idx_pos;
    if (player->idx_pos >= PLAYER_MAX_POSITIONS)
    {
        player->idx_pos = 0;
    }
    player->positions[player->idx_pos] = prev_pos;

    // check for going into yourself
    for (int32_t i = player->idx_pos - 1, cells = 1; cells < player->length; ++cells)
    {
        if (i < 0)
            i = PLAYER_MAX_POSITIONS - 1;

        if (player->positions[i].x == prev_pos.x && player->positions[i].y == prev_pos.y)
        {
            return true;
        }
        --i;
    }
    return false;
}

static void draw_food(const Food *food)
{
    draw_block_at(food->pos, GREEN);
}

static void food_init_position(Food *food)
{
    Coord x = GetRandomValue(0, WIDTH - 1);
    Coord y = GetRandomValue(0, HEIGHT - 1);
    food->pos.x = x;
    food->pos.y = y;
}

static void food_player_collision_logic(Player *player, Food *food)
{
    if (food->pos.x == player->positions[player->idx_pos].x && food->pos.y == player->positions[player->idx_pos].y)
    {
        ++player->length;
        food_init_position(food);
    }
}

static void evil_snake_move(Evil_Snake *snake)
{
    for (int i = 0; i < snake->length; ++i)
    {
        snake->positions[i] = move_inside_grid(snake->positions[i], snake->direction);
    }
}

static void evil_snakes_draw(const Evil_Snake snakes[], const int32_t length)
{
    for (int32_t i = 0; i < length; ++i)
    {
        const Evil_Snake *snake = &snakes[i];
        for (int32_t j = 0; j < snake->length; ++j)
        {
            draw_block_at(snake->positions[j], BLUE);
        }
    }
}

// returns true on collision
static bool evil_snakes_player_collision_logic(const Evil_Snake snakes[], const int32_t length, const Player *player)
{
    for (int32_t snake_idx = 0; snake_idx < length; ++snake_idx)
    {
        const Evil_Snake *snake = &snakes[snake_idx];

        for (int i = 0; i < 1; ++i) // only collide with the head
        {
            for (int j = 0; j < snake->length; ++j)
            {
                if (pos_equal(player_nth_position(player, i), snake->positions[j]))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

static void game_loop()
{

GOTO_START:;

    Player player = {.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing};
    player.positions[0] = (Pos){.x = START_X, .y = START_Y};

    Food food;
    food_init_position(&food);

    double time_for_move = 1.0;

    const int32_t total_evil_snakes = 10;
    // index to awaken next snake
    int32_t evil_snake_index = 0;

    Evil_Snake evil_snakes[total_evil_snakes];

    while (!WindowShouldClose())
    {
        // logic
        if (IsKeyPressed(KEY_A))
        {
            TraceLog(LOG_INFO, "%s", "also this works!");
        }

        bool should_move = false;
        {
            double time = GetTime();
            if (time >= time_for_move)
            {
                time_for_move = time + 0.1;
                should_move = true;
            }
        }

        player_set_direction(&player);

        if (should_move)
        {
            if (player_move(&player))
            {
                // player died
                TraceLog(LOG_INFO, "%s", "YOU DIED!");
                goto GOTO_START;
            }

            if (evil_snakes_player_collision_logic(evil_snakes, evil_snake_index, &player))
            {
                goto GOTO_START;
            }

            for (int32_t i = 0; i < evil_snake_index; ++i)
                evil_snake_move(&evil_snakes[i]);

            food_player_collision_logic(&player, &food);

            // spawn evil snakes
            if (player.length > 2)
            {
                if (GetRandomValue(1, 10) == 1 && evil_snake_index < total_evil_snakes)
                {
                    // spawn
                    evil_snakes[evil_snake_index] = (Evil_Snake){.length = 2, Dir_Right, {{0, 5}, {1, 5}}};
                    Evil_Snake *snake = &evil_snakes[evil_snake_index];

                    Pos start;
                    Pos tail_dir;
                    Dir dir;

                    switch (GetRandomValue(1, 4))
                    {
                    case 1: // from left
                        start = (Pos){.x = -1, .y = GetRandomValue(0, HEIGHT - 1)};
                        tail_dir = (Pos){-1, 0};
                        dir = Dir_Right;
                        break;
                    case 2: // from right
                        start = (Pos){.x = WIDTH, .y = GetRandomValue(0, HEIGHT - 1)};
                        tail_dir = (Pos){1, 0};
                        dir = Dir_Left;
                        break;
                    case 3: // from top
                        start = (Pos){.x = GetRandomValue(0, WIDTH - 1), .y = -1};
                        tail_dir = (Pos){0, -1};
                        dir = Dir_Down;
                        break;
                    case 4: // from bottom
                        start = (Pos){.x = GetRandomValue(0, WIDTH - 1), .y = HEIGHT};
                        tail_dir = (Pos){0, 1};
                        dir = Dir_Up;
                        break;
                    }
                    snake->length = GetRandomValue(2, EVIL_SNAKE_MAX_LENGTH);
                    for (int32_t i = 0; i < snake->length; ++i)
                    {
                        snake->positions[i] = (Pos){start.x + tail_dir.x * i, start.y + tail_dir.y * i};
                    }
                    snake->direction = dir;
                    ++evil_snake_index;
                }
            }
        }

        // drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        player_draw(&player);
        evil_snakes_draw(evil_snakes, evil_snake_index);
        draw_food(&food);

        char myText[100];
        int fps = GetFPS();
        snprintf(myText, sizeof(myText), "FPS: %d", fps);
        DrawText(myText, 10, 10, 20, LIGHTGRAY);
        EndDrawing();
    }
}

int main(void)
{
    InitWindow(WIDTH * BLOCK_PIXEL_LEN, HEIGHT * BLOCK_PIXEL_LEN, "CSnake");

    SetTargetFPS(GAME_FPS);

    game_loop();

    CloseWindow();

    return 0;
}
