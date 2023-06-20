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
            color = ((i) % 2 == 0) ? ORANGE : MAROON;
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

typedef struct
{
    World_State0 w;
    Player player;
    Food food;
    double time_for_move;
} Game_State0;

static Game_State0 game_state0_init(void)
{
    Game_State0 g;
    g.w = world_state0_init(12);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Nothing};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.w);
    g.time_for_move = 1.0;
    return g;
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

static Game_State1 game_state1_init(void)
{
    Game_State1 g;
    g.w = world_state0_init(28);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Nothing};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.w);
    g.time_for_move = 1.0;
    g.evil_snake_index = 0;

    return g;
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

static void box_player_collision_logic(Box *box, const Player *player)
{
    Pos p = player_nth_position(player, 0);
    Pos b = box->p, w_h = box->w_h;
    if (p.x >= b.x && p.x < b.x + w_h.x && p.y >= b.y && p.y < b.y + w_h.y)
    {
        Pos dir_pos = dir_to_pos(player->current_direction);
        box->p.x += dir_pos.x;
        box->p.y += dir_pos.y;
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

static Game_State2 game_state2_init(void)
{
    Game_State2 g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 3, .idx_pos = 2, .current_direction = Dir_Right, .next_direction = Dir_Nothing};
    g.player.positions[2] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player.positions[1] = (Pos){.x = g.w.width / 2 - 1, g.w.height / 2};
    g.player.positions[0] = (Pos){.x = g.w.width / 2 - 2, g.w.height / 2};

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {1, 1}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};

    g.time_for_move = 1.0;

    return g;
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

static Int game_state0_frame0(Game_State0 *g)
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

            *g = game_state0_init();
            return 0;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = (DEV ? 2 : 6) - g->player.length;

    if (food_left_to_win <= 0)
        return 1;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw(&g->player, w);
    draw_food(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}

static Int game_state0_frame1(Game_State0 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            *g = game_state0_init();
            return 0;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = (DEV ? 2 : 12) - g->player.length;
    if (food_left_to_win <= 0)
        return 1;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    draw_food(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}

typedef struct Game_Cutscene0
{
    double start_time;

} Game_Cutscene0;

static Game_Cutscene0 game_cutscene0_init(void)
{
    Game_Cutscene0 g;
    g.start_time = GetTime();
    return g;
}

static Int game_cutscene0_frame0(Game_Cutscene0 *g)
{
    double time_passed = GetTime() - g->start_time;

    if (time_passed > 2.f)
        return 1;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(time_passed * 500, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }
    EndDrawing();

    return 0;
}

static Int game_cutscene0_frame1(Game_Cutscene0 *g)
{
    double time_passed = GetTime() - g->start_time;

    if (time_passed > 2.f)
        return 1;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(WINDOW_WIDTH - time_passed * 500, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }
    EndDrawing();

    return 0;
}

static Int game_state1_frame0(Game_State1 *g)
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

            *g = game_state1_init();
            return 0;
        }

        if (evil_snakes_player_collision_logic(g->evil_snakes, g->evil_snake_index, &g->player))
        {
            *g = game_state1_init();
            return 0;
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
        return 1;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw(&g->player, w);
    evil_snakes_draw(g->evil_snakes, g->evil_snake_index, w);
    draw_food(&g->food, w);

    draw_fps();
    EndDrawing();
    return 0;
}

static Int game_state2_frame0(Game_State2 *g)
{
    // logic
    player_set_direction(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            *g = game_state2_init();
            return 0;
        }

        for (Int i = 0; i < GAME_STATE2_BOXES; ++i)
        {
            box_player_collision_logic(&g->boxes[i], &g->player);
        }
    }

    Int food_left_to_win = (DEV ? 6 : 6) - g->player.length;

    if (food_left_to_win <= 0)
        return 1;
    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw(&g->player, &g->w);
    for (Int i = 0; i < GAME_STATE2_BOXES; i++)
    {
        box_draw(&g->boxes[i], &g->w);
    }

    draw_fps();
    EndDrawing();
    return 0;
}

typedef Int (*Meta_Game_Frame_Code)(void *);

typedef struct Meta_Game
{
    Meta_Game_Frame_Code frame_code;
    void *data;
    Int frame;
} Meta_Game;

static Meta_Game meta_game_init(Int frame)
{
    Meta_Game mg;

    if (DEV)
    {
        if (frame < 5)
            frame = 6;
    }
    mg.frame = frame;

    switch (frame)
    {
    case 0: {
        mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame0;
        mg.data = malloc(sizeof(Game_State0));
        *((Game_State0 *)mg.data) = game_state0_init();
    }
    break;
    case 1: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.data = malloc(sizeof(Game_Cutscene0));
        *(Game_Cutscene0 *)mg.data = game_cutscene0_init();
    }
    break;
    case 2: {
        mg.frame_code = (Meta_Game_Frame_Code)game_state1_frame0;
        mg.data = malloc(sizeof(Game_State1));
        *((Game_State1 *)mg.data) = game_state1_init();
    }
    break;
    case 3: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame1;
        mg.data = malloc(sizeof(Game_Cutscene0));
        *(Game_Cutscene0 *)mg.data = game_cutscene0_init();
    }
    break;
    case 4: {
        mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame1;
        mg.data = malloc(sizeof(Game_State0));
        *((Game_State0 *)mg.data) = game_state0_init();
    }
    break;
    case 5: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.data = malloc(sizeof(Game_Cutscene0));
        *(Game_Cutscene0 *)mg.data = game_cutscene0_init();
    }
    break;
    case 6: { // boxes
        mg.frame_code = (Meta_Game_Frame_Code)game_state2_frame0;
        mg.data = malloc(sizeof(Game_State2));
        *((Game_State2 *)mg.data) = game_state2_init();
    }
    break;
    case 7: {
        printf("VERY BAD DEATH!!!!!!!!! AHHHHHHHH LEVEL NOT EXIST");
    }
    break;
    }

    return mg;
}

static void meta_game_frame(Meta_Game *mg)
{
    if (mg->frame_code(mg->data))
    {
        free(mg->data);
        *mg = meta_game_init(mg->frame + 1);
    }
}

// ALLOCATES!
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
