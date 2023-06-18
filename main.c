#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define GAME_FPS 144

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

// #define HEIGHT 25
// #define WIDTH 30
// #define BLOCK_PIXEL_LEN 25

// #define START_X (WIDTH / 2)
// #define START_Y (HEIGHT / 2)

typedef int8_t Coord; // Coord in world

typedef int32_t Int; // for general things like indexing arrays and stuff

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

typedef struct
{
    // Cells in current world
    Coord width, height;
    Int block_pixel_len;
} World_State0;

World_State0 world_state0_init()
{
    World_State0 w = {.width = 30, .height = 25, .block_pixel_len = 20};
    w.block_pixel_len = GetRandomValue(20, 25);
    SetWindowSize(w.block_pixel_len * w.width, w.block_pixel_len * w.height);
    return w;
}

#define PLAYER_MAX_POSITIONS (100)
typedef struct Player
{
    Int idx_pos;
    Int length;
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
    Int length;
    Dir direction;
    Pos positions[EVIL_SNAKE_MAX_LENGTH];
} Evil_Snake;

static bool pos_equal(Pos p, Pos q)
{
    return p.x == q.x && p.y == q.y;
}

static void draw_block_at(Pos pos, Color color, const World_State0 *w)
{
    DrawRectangle(pos.x * w->block_pixel_len, pos.y * w->block_pixel_len, w->block_pixel_len, w->block_pixel_len,
                  color);
}

static Pos player_nth_position(const Player *player, Int idx)
{
    Int i = player->idx_pos - idx;
    while (i < 0)
        i += PLAYER_MAX_POSITIONS;
    return player->positions[i];
}
// Cycle through positions of player
static void player_draw(const Player *player, const World_State0 *w)
{
    Int drawn_cells = 0;
    for (Int i = player->idx_pos; drawn_cells < player->length; ++drawn_cells)
    {
        const Pos *pos = &player->positions[i];
        draw_block_at(*pos, (drawn_cells == 0) ? RED : MAROON, w);

        --i;
        if (i < 0)
        {
            i = PLAYER_MAX_POSITIONS - 1;
        }
    }
}

static void player_draw_extra(const Player *player, const World_State0 *w)
{
    for (Int i = 0; i < player->length; ++i)
    {
        const Pos pos = player_nth_position(player, i);
        draw_block_at(pos, RED, w);
    }
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

static Pos move_inside_grid(Pos pos, const Dir dir, World_State0 *w)
{
    switch (dir)
    {
    case Dir_Right:
        ++pos.x;
        if (pos.x >= w->width)
            pos.x = 0;
        break;
    case Dir_Left:
        --pos.x;
        if (pos.x < 0)
            pos.x = w->width - 1;
        break;
    case Dir_Up:
        --pos.y;
        if (pos.y < 0)
            pos.y = w->height - 1;
        break;
    case Dir_Down:
        ++pos.y;
        if (pos.y >= w->height)
            pos.y = 0;
        break;
    case Dir_Nothing:
        break;
    }
    return pos;
}

// returns true if player dies
static bool player_move(Player *player, World_State0 *w)
{
    Pos prev_pos = player->positions[player->idx_pos];

    // change prev pos to new position
    player->current_direction = player->next_direction;
    if (player->current_direction == Dir_Nothing)
        return false;
    prev_pos = move_inside_grid(prev_pos, player->current_direction, w);

    ++player->idx_pos;
    if (player->idx_pos >= PLAYER_MAX_POSITIONS)
    {
        player->idx_pos = 0;
    }
    player->positions[player->idx_pos] = prev_pos;

    // check for going into yourself
    for (Int i = player->idx_pos - 1, cells = 1; cells < player->length; ++cells)
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

static void draw_food(const Food *food, const World_State0 *w)
{
    draw_block_at(food->pos, GREEN, w);
}

static void food_init_position(Food *food, const World_State0 *w)
{
    Coord x = GetRandomValue(0, w->width - 1);
    Coord y = GetRandomValue(0, w->height - 1);
    food->pos.x = x;
    food->pos.y = y;
}

static void food_player_collision_logic(Player *player, Food *food, World_State0 *w)
{
    if (food->pos.x == player->positions[player->idx_pos].x && food->pos.y == player->positions[player->idx_pos].y)
    {
        ++player->length;
        food_init_position(food, w);
    }
}

static void evil_snake_move(Evil_Snake *snake, World_State0 *w)
{
    for (int i = 0; i < snake->length; ++i)
    {
        snake->positions[i] = move_inside_grid(snake->positions[i], snake->direction, w);
    }
}

static void evil_snakes_draw(const Evil_Snake snakes[], const Int length, const World_State0 *w)
{
    for (Int i = 0; i < length; ++i)
    {
        const Evil_Snake *snake = &snakes[i];
        for (Int j = 0; j < snake->length; ++j)
        {
            draw_block_at(snake->positions[j], (j == 0) ? SKYBLUE : BLUE, w);
        }
    }
}

// returns true on collision
static bool evil_snakes_player_collision_logic(const Evil_Snake snakes[], const Int length, const Player *player)
{
    for (Int snake_idx = 0; snake_idx < length; ++snake_idx)
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

#define GAME_STATE0_TOTAL_EVIL_SNAKES 10
typedef struct Game_State0
{
    Player player;
    Food food;
    double time_for_move;
    Int evil_snake_index;
    Evil_Snake evil_snakes[GAME_STATE0_TOTAL_EVIL_SNAKES];
} Game_State0;

typedef struct
{
    Game_State0 g;
    World_State0 w;
} Game_And_World_State0;

Game_State0 game_state0_init(const World_State0 *w)
{
    Game_State0 g;
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Nothing};
    g.player.positions[0] = (Pos){.x = w->width / 2, w->height / 2};
    food_init_position(&g.food, w);
    g.time_for_move = 1.0;
    g.evil_snake_index = 0;

    return g;
}

void game_state0_frame0(Game_And_World_State0 *gw)
{
    Game_State0 *g = &gw->g;
    World_State0 *w = &gw->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    bool should_move = false;
    {
        double time = GetTime();
        if (time >= g->time_for_move)
        {
            g->time_for_move = time + 0.1;
            should_move = true;
        }
    }

    player_set_direction(&g->player);

    if (should_move)
    {
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            *w = world_state0_init();
            *g = game_state0_init(w);
            return;
        }

        if (evil_snakes_player_collision_logic(g->evil_snakes, g->evil_snake_index, &g->player))
        {
            *w = world_state0_init();
            *g = game_state0_init(w);
            return;
        }

        for (Int i = 0; i < g->evil_snake_index; ++i)
            evil_snake_move(&g->evil_snakes[i], w);

        food_player_collision_logic(&g->player, &g->food, w);

        // spawn evil snakes
        if (g->evil_snake_index < g->player.length - 2)
        {
            if (GetRandomValue(1, 1) == 1 && g->evil_snake_index < GAME_STATE0_TOTAL_EVIL_SNAKES)
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

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    player_draw(&g->player, w);
    evil_snakes_draw(g->evil_snakes, g->evil_snake_index, w);
    draw_food(&g->food, w);

    char myText[100];
    int fps = GetFPS();
    snprintf(myText, sizeof(myText), "FPS: %d", fps);
    DrawText(myText, 10, 10, 20, LIGHTGRAY);
    EndDrawing();
}

typedef struct
{
    void (*frame_code)(void *);
    void *data;

} Meta_Game;

void meta_game_frame(Meta_Game *mg)
{
    mg->frame_code(mg->data);
}

// ALLOCATES!
Meta_Game meta_game_init()
{
	Meta_Game mg;
	mg.frame_code = (void (*)(void *))game_state0_frame0;
	mg.data = malloc(sizeof (Game_And_World_State0));

    Game_And_World_State0 *gw = (Game_And_World_State0*)mg.data;
    gw->w = world_state0_init();
    gw->g = game_state0_init(&gw->w);
	return mg;
}

static void game_loop()
{

	Meta_Game mg = meta_game_init();
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg((void (*)(void *))meta_game_frame, &mg, 0, 1);

#else
    SetTargetFPS(GAME_FPS); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
		meta_game_frame(&mg);
    }
#endif
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CSnake");

    SetTargetFPS(GAME_FPS);

    game_loop();

    CloseWindow();

    return 0;
}
