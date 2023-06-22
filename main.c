#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define DEV 1
#define GAME_FPS 144

#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT (630)

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

typedef struct World_State0
{
    // Cells in current world
    Coord width, height;
    Int block_pixel_len;
} World_State0;

#define PLAYER_MAX_POSITIONS (300)
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

static Pos dir_to_pos(Dir d)
{
    switch (d)
    {
    case Dir_Right:
        return (Pos){1, 0};
    case Dir_Left:
        return (Pos){-1, 0};
    case Dir_Up:
        return (Pos){0, -1};
    case Dir_Down:
        return (Pos){0, 1};
    case Dir_Nothing:
        return (Pos){0, 0};
    }
    abort();
}

static void draw_block_at(Pos pos, Color color, const World_State0 *w)
{
    DrawRectangle(pos.x * w->block_pixel_len, pos.y * w->block_pixel_len, w->block_pixel_len, w->block_pixel_len,
                  color);
}
static void draw_blocks_at(Pos pos, Pos w_h, Color color, const World_State0 *w)
{
    DrawRectangle(pos.x * w->block_pixel_len, pos.y * w->block_pixel_len, w->block_pixel_len * w_h.x,
                  w->block_pixel_len * w_h.y, color);
}

World_State0 world_state0_init(Int width)
{
    World_State0 w = {.width = width};
    w.block_pixel_len = WINDOW_WIDTH / w.width;
    w.height = WINDOW_HEIGHT / w.block_pixel_len;
    return w;
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

        Color color;
        if (i == 0)
        {
            color = RED;
        }
        else
        {
            color = (i % 6 == 0) ? GOLD : (i % 2 == 0) ? ORANGE : MAROON;
        }
        draw_block_at(pos, color, w);
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

static Pos move_inside_grid(Pos pos, const Dir dir, const World_State0 *w)
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

static bool player_intersection_point(const Player *player, const Pos point)
{
    for (Int i = 0; i < player->length; ++i)
    {
        Pos p_pos = player_nth_position(player, i);
        if (pos_equal(p_pos, point))
            return true;
    }
    return false;
}

static void food_draw(const Food *food, const World_State0 *w)
{
    draw_block_at(food->pos, GREEN, w);
}

static void food_set_random_position(Food *food, const World_State0 *w)
{
    Coord x = GetRandomValue(0, w->width - 1);
    Coord y = GetRandomValue(0, w->height - 1);
    food->pos.x = x;
    food->pos.y = y;
}
static void food_init_position(Food *food, const Player *player, const World_State0 *w)
{
    do
    {
        food_set_random_position(food, w);
    } while (player_intersection_point(player, food->pos));
}

static void food_player_collision_logic(Player *player, Food *food, const World_State0 *w)
{
    if (pos_equal(player_nth_position(player, 0), food->pos))
    {
        ++player->length;
        food_init_position(food, player, w);
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

typedef enum Level_Return
{
    Level_Return_Continue = 0,
    Level_Return_Next_Level,
    Level_Return_Reset_Level,
} Level_Return;

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
} Game_State0;

static void game_state0_init0(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(12);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

#define GAME_STATE1_TOTAL_EVIL_SNAKES 10
typedef struct Game_State1
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
    Int evil_snake_index;
    Evil_Snake evil_snakes[GAME_STATE1_TOTAL_EVIL_SNAKES];
} Game_State1;

static void game_state1_init(Game_State1 *new_g)
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

typedef struct
{
    Pos p;
    // width and height
    Pos w_h;
} Box;

static void box_draw(Box *b, World_State0 *w)
{
    draw_blocks_at(b->p, b->w_h, PINK, w);
}

static bool rect_intersection(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2)
{
    return r1.x < r2.x + w_h2.x && r1.x + w_h1.x > r2.x && r1.y < r2.y + w_h2.y && r1.y + w_h1.y > r2.y;
}

// For boxes, and their number, we have a position and a direction toghether with a width and height, that will move the
// boxes, excluding one index of boxes. Also world
static void boxes_player_collision_logic(Box boxes[], const Int nr_boxes, Pos pos, Dir dir, Pos w_h, Int exclude_idx,
                                         const World_State0 *w)
{
    for (Int box_idx = 0; box_idx < nr_boxes; ++box_idx)
    {
        if (box_idx == exclude_idx)
            continue;

        Box *box = &boxes[box_idx];

        if (rect_intersection(box->p, box->w_h, pos, w_h))
        {
            box->p = move_inside_grid(box->p, dir, w);
            boxes_player_collision_logic(boxes, nr_boxes, box->p, dir, box->w_h, box_idx, w);
        }
    }
}

#define GAME_STATE2_BOXES 2
typedef struct
{
    World_State0 w;
    Player player;
    Box boxes[GAME_STATE2_BOXES];
    double time_for_move;
} Game_State2;

static void game_state2_init(Game_State2 *new_g)
{
    Game_State2 g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 5, .idx_pos = 4, .current_direction = Dir_Right, .next_direction = Dir_Right};
    g.player.positions[4] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player.positions[3] = (Pos){.x = g.w.width / 2 - 1, g.w.height / 2};
    g.player.positions[2] = (Pos){.x = g.w.width / 2 - 2, g.w.height / 2};
    g.player.positions[1] = (Pos){.x = g.w.width / 2 - 3, g.w.height / 2};
    g.player.positions[0] = (Pos){.x = g.w.width / 2 - 4, g.w.height / 2};

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {1, 1}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};

    g.time_for_move = 1.0;

    *new_g = g;
}

typedef struct
{
    World_State0 w;
    Player player;
    Int player_points;
    Food food;
    double time_for_move;
} Game_State3;

static void game_state3_init0(Game_State3 *new_g)
{
    Game_State3 g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player_points = 0;
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

#define GAME_STATE4_BOXES 20
typedef struct
{
    World_State0 w;
    Player player;
	Food food;
    Box boxes[GAME_STATE4_BOXES];
    double time_for_move;
} Game_State4;

static void game_state4_init(Game_State4 *new_g)
{
    Game_State4 g;
    g.w = world_state0_init(64);
    g.player = (Player){.length = 5, .idx_pos = 4, .current_direction = Dir_Right, .next_direction = Dir_Right};
    g.player.positions[4] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player.positions[3] = (Pos){.x = g.w.width / 2 - 1, g.w.height / 2};
    g.player.positions[2] = (Pos){.x = g.w.width / 2 - 2, g.w.height / 2};
    g.player.positions[1] = (Pos){.x = g.w.width / 2 - 3, g.w.height / 2};
    g.player.positions[0] = (Pos){.x = g.w.width / 2 - 4, g.w.height / 2};

	food_init_position(&g.food, &g.player, &g.w);

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {1, 1}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};

    g.time_for_move = 1.0;

    *new_g = g;
}

static void draw_fps(void)
{
    char myText[100];
    int fps = GetFPS();
    snprintf(myText, sizeof(myText), "FPS: %d", fps);
    DrawText(myText, 10, 10, 20, LIGHTGRAY);
}

static void draw_food_left(Int food_left_to_win)
{
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d", food_left_to_win);
    DrawText(buffer, 200, -40, 800, (Color){0, 0, 0, 40});
}

static bool time_move_logic(double *time_for_move)
{
    double time = GetTime();
    if (time >= *time_for_move)
    {
        *time_for_move = time + 0.1;
        return true;
    }
    return false;
}

// normal snake
static Level_Return game_state0_frame0(Game_State0 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction(&g->player);

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

    Level_Return food_left_to_win = (DEV ? 2 : 6) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

// flashier snake
static Level_Return game_state0_frame1(Game_State0 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = (DEV ? 2 : 12) - g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

// ever expanding snake
static Level_Return game_state3_frame0(Game_State3 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        Int player_len = g->player.length;

        g->player.length++;
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        g->player.length--;
        if (pos_equal(player_nth_position(&g->player, 0), g->food.pos))
        {
            food_init_position(&g->food, &g->player, w);
			g->player_points++;
        }
        g->player.length = player_len + 1;

        // printf("player len: %d\n", g->player.length);
    }

    Int points_left = (DEV ? 6 : 6) - g->player_points;
    if (points_left == 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(points_left);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

typedef struct Game_Cutscene0
{
    double start_time;

} Game_Cutscene0;

static void game_cutscene0_init(Game_Cutscene0 *new_g)
{
    Game_Cutscene0 g;
    g.start_time = GetTime();
    *new_g = g;
}

static Level_Return game_cutscene0_frame0(Game_Cutscene0 *g)
{
    double time_passed = GetTime() - g->start_time;

    if (time_passed > 2.f)
        return Level_Return_Next_Level;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(time_passed * 500, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }
    EndDrawing();

    return Level_Return_Continue;
}

static Level_Return game_cutscene0_frame1(Game_Cutscene0 *g)
{
    double time_passed = GetTime() - g->start_time;

    if (time_passed > 2.f)
        return Level_Return_Next_Level;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(WINDOW_WIDTH - time_passed * 500, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }
    EndDrawing();

    return Level_Return_Continue;
}

static Level_Return game_state1_frame0(Game_State1 *g)
{
    World_State0 *w = &g->w;
    // logic
    if (IsKeyPressed(KEY_A))
    {
        TraceLog(LOG_INFO, "%s", "also this works!");
    }

    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        if (evil_snakes_player_collision_logic(g->evil_snakes, g->evil_snake_index, &g->player))
        {
            return Level_Return_Reset_Level;
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
    evil_snakes_draw(g->evil_snakes, g->evil_snake_index, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}

static Level_Return game_state2_frame0(Game_State2 *g)
{
    // logic
    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        boxes_player_collision_logic(g->boxes, GAME_STATE2_BOXES, player_nth_position(&g->player, 0),
                                     g->player.current_direction, (Pos){1, 1}, -1, &g->w);
    }

    Pos pos1 = {20, 15};
    Pos pos2 = {5, 5};

    Int points = 0;

    for (Int i = 0; i < GAME_STATE2_BOXES; ++i)
    {
        if (pos_equal(pos1, g->boxes[i].p) || pos_equal(pos2, g->boxes[i].p))
        {
            ++points;
        }
    }

    if (points == 2)
    {
        return Level_Return_Next_Level;
    }

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(2 - points);
    player_draw_extra(&g->player, &g->w);
    draw_block_at(pos1, GREEN, &g->w);
    draw_block_at(pos2, GREEN, &g->w);
    for (Int i = 0; i < GAME_STATE2_BOXES; i++)
    {
        box_draw(&g->boxes[i], &g->w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

// Boxes hiding food
static Level_Return game_state4_frame0(Game_State4 *g)
{
    // logic
    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        boxes_player_collision_logic(g->boxes, GAME_STATE2_BOXES, player_nth_position(&g->player, 0),
                                     g->player.current_direction, (Pos){1, 1}, -1, &g->w);

		food_player_collision_logic(&g->player, &g->food, &g->w);
    }
    Int food_left_to_win = (DEV ? 8 : 8) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw_extra(&g->player, &g->w);
	food_draw(&g->food, &g->w);
    for (Int i = 0; i < GAME_STATE2_BOXES; i++)
    {
        box_draw(&g->boxes[i], &g->w);
    }

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

typedef Level_Return (*Meta_Game_Frame_Code)(void *);
typedef void (*Meta_Game_Init_Code)(void *);

typedef struct Meta_Game
{
    Meta_Game_Frame_Code frame_code;
    Meta_Game_Init_Code init_code;

    void *data;
    Int frame;
} Meta_Game;

static Meta_Game meta_game_init(Int frame)
{
    Meta_Game mg;

    if (DEV)
    {
        Int skip = 10;
        if (frame < skip)
            frame = skip;
    }
    mg.frame = frame;

    switch (frame)
    {
    case 0: {
        mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_state0_init0;
        mg.data = malloc(sizeof(Game_State0));
    }
    break;
    case 1: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 2: {
        mg.frame_code = (Meta_Game_Frame_Code)game_state1_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_state1_init;
        mg.data = malloc(sizeof(Game_State1));
    }
    break;
    case 3: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame1;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 4: {
        mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame1;
        mg.init_code = (Meta_Game_Init_Code)game_state0_init0;
        mg.data = malloc(sizeof(Game_State0));
    }
    break;
    case 5: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 6: { // boxes
        mg.frame_code = (Meta_Game_Frame_Code)game_state2_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_state2_init;
        mg.data = malloc(sizeof(Game_State2));
    }
    break;
    case 7: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 8: { // ever growing
        mg.frame_code = (Meta_Game_Frame_Code)game_state3_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_state3_init0;
        mg.data = malloc(sizeof(Game_State3));
    }
    break;
    case 9: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 10: { // ever growing
        mg.frame_code = (Meta_Game_Frame_Code)game_state4_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_state4_init;
        mg.data = malloc(sizeof(Game_State4));
    }
    break;
    default: {
        printf("VERY BAD DEATH!!!!!!!!! AHHHHHHHH LEVEL NOT EXIST\n");
    }
    break;
    }
    mg.init_code(mg.data);

    return mg;
}

static void meta_game_frame(Meta_Game *mg)
{
    switch (mg->frame_code(mg->data))
    {
    case Level_Return_Continue: {
    }
    break;
    case Level_Return_Next_Level: {
        TraceLog(LOG_INFO, "%s", "THE FUNCTION RETURNED ONE ONE ONE \n");
        free(mg->data);
        *mg = meta_game_init(mg->frame + 1);
    }
    break;
    case Level_Return_Reset_Level: {
        mg->init_code(mg->data);
    }
    break;
    }
}

static void game_loop(void)
{
    Meta_Game mg = meta_game_init(0);
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
