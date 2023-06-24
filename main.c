#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
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
    Dir_Right = 0, // obligatory numbers for first 4, as YouFood level generates random dir
    Dir_Left = 1,
    Dir_Up = 2,
    Dir_Down = 3,
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

// will not warp
static void draw_blocks_at(Pos pos, Pos w_h, Color color, const World_State0 *w)
{
    DrawRectangle(pos.x * w->block_pixel_len, pos.y * w->block_pixel_len, w->block_pixel_len * w_h.x,
                  w->block_pixel_len * w_h.y, color);
}

// draw warping, assuming it can not have negative position(ex: boxes) and will only overflow at width and height of
// world
static void draw_blocks_warp(Pos pos, Pos w_h, Color color, const World_State0 *w)
{
    draw_blocks_at(pos, w_h, color, w);
    Coord x_over = pos.x + w_h.x - w->width;
    Coord y_over = pos.y + w_h.y - w->height;
    if (x_over > 0)
    {
        draw_blocks_at((Pos){0, pos.y}, (Pos){x_over, w_h.y}, color, w);
    }
    if (y_over > 0)
    {
        draw_blocks_at((Pos){pos.x, 0}, (Pos){w_h.x, y_over}, color, w);
    }
    if (x_over > 0 && y_over > 0) // special case for when at width and height
    {
        draw_blocks_at((Pos){0, 0}, (Pos){x_over, y_over}, color, w);
    }
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

// NOT PURE
static Dir get_dir_from_input(void)
{
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_L))
    {
        return Dir_Right;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_H))
    {
        return Dir_Left;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_K))
    {
        return Dir_Up;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_J))
    {
        return Dir_Down;
    }
    return Dir_Nothing;
}

// do not turn in a way that instantly kills youself, handles next and current direction
static void player_set_direction_correctly(Player *player, Dir dir)
{
    switch (dir)
    {
    case Dir_Right: {
        if (player->current_direction != Dir_Left)
            player->next_direction = Dir_Right;
    }
    break;
    case Dir_Left: {
        if (player->current_direction != Dir_Right)
            player->next_direction = Dir_Left;
    }
    break;
    case Dir_Up: {
        if (player->current_direction != Dir_Down)
            player->next_direction = Dir_Up;
    }
    break;
    case Dir_Down: {
        if (player->current_direction != Dir_Up)
            player->next_direction = Dir_Down;
    }
    case Dir_Nothing: {
    }
    break;
    }
}

static void player_set_direction_from_input(Player *player)
{
    player_set_direction_correctly(player, get_dir_from_input());
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

static void evil_snake_draw(const Evil_Snake *snake, const World_State0 *w)
{
    const Int length = snake->length;
    for (Int j = 0; j < length; ++j)
    {
        draw_block_at(snake->positions[j], (j == 0) ? SKYBLUE : BLUE, w);
    }
}

// returns true on collision
static bool evil_snake_player_collision_logic(const Evil_Snake *snake, const Player *player)
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

static bool time_move_logic_general(double *time_for_move, const double wait_time)
{
    double time = GetTime();
    if (time >= *time_for_move)
    {
        *time_for_move = time + wait_time;
        return true;
    }
    return false;
}

static bool time_move_logic(double *time_for_move)
{
    return time_move_logic_general(time_for_move, 0.1);
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

// Gigantic free fast
static void game_state0_init1(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(120);
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
    World_State0 w;
    Player player;
    Food food;
    Dir food_dir;
    double time_for_move;
} Game_State_YouFood;

static void game_state_YouFood_init(Game_State_YouFood *new_g)
{
    Game_State_YouFood g;
    g.w = world_state0_init(24);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.food_dir = Dir_Nothing;
    g.time_for_move = 1.0;

    *new_g = g;
}

static Level_Return game_state_YouFood_frame(Game_State_YouFood *g)
{
    World_State0 *w = &g->w;
    // logic

    {
        Dir new_dir = get_dir_from_input();
        if (new_dir != Dir_Nothing)
            g->food_dir = new_dir;
    }

    if (time_move_logic_general(&g->time_for_move, 0.135))
    {
        if (GetRandomValue(1, 3) == 1)
            player_set_direction_correctly(&g->player, GetRandomValue(0, 3));
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            // return Level_Return_Reset_Level;
        }
        g->food.pos = move_inside_grid(g->food.pos, g->food_dir, w);
        g->food_dir = Dir_Nothing;
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Level_Return food_left_to_win = (DEV ? 7 : 7) - g->player.length;

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

typedef enum
{
    Maze_Cell_Empty = 0,
    Maze_Cell_Wall,
} Maze_Cell;

static void maze_draw(const Maze_Cell maze[], const Int maze_width, const Int maze_height, const World_State0 *w)
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

typedef struct
{
    Dir dir;
    Coord len;
} Snake_Pather_Way;

#define SNAKE_PATHER_MAX_WAYS 20
#define SNAKE_PATHER_MAX_LEN 10
typedef struct
{
    Pos positions[SNAKE_PATHER_MAX_LEN];
    Coord len;
    // ways to go in path
    Snake_Pather_Way ways[SNAKE_PATHER_MAX_WAYS];
    // CONST. how many actual ways you have
    Int ways_len;
    // what way are we currently on ^
    Int way_idx;
    // Starts high, goes to zero, depending on Way::len
    Int walk_this_way_counter;
} Snake_Pather;

static void snake_pather_draw(Snake_Pather *snake_pather, World_State0 *w)
{
    for (Int i = 0; i < snake_pather->len; ++i)
    {
        draw_block_at(snake_pather->positions[i], BLUE, w);
    }
}

static void snake_pather_move(Snake_Pather *snake_pather, World_State0 *w)
{
    // get new way
    printf("path move counter: %d\n", snake_pather->walk_this_way_counter);
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

static bool snake_pather_player_intersection(Snake_Pather *snake_pather, Player *player)
{
    for (Int i = 0; i < player->length; ++i)
    {
        for (Int j = 0; j < snake_pather->len; ++j)
        {
            if (pos_equal(snake_pather->positions[j], player_nth_position(player, i))) {
				return true;
			}
        }
    }
	return false;
}

#define GAME_STATE_MAZE_FOODS 4
#define GAME_STATE_MAZE_WIDTH 40 // you CANT change these, hardcoded
#define GAME_STATE_MAZE_HEIGHT 30
#define GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS 2
typedef struct
{
    World_State0 w;
    Player player;
    double time_for_move;
    Food foods[GAME_STATE_MAZE_FOODS];
    Snake_Pather evil_snake_paths[GAME_STATE_MAZE_TOTAL_EVIL_SNAKE_PATHS];
    Maze_Cell maze[GAME_STATE_MAZE_HEIGHT][GAME_STATE_MAZE_WIDTH];
} Game_State_Maze;

static void game_state_Maze_init(Game_State_Maze *new_g)
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

static Level_Return game_state_Maze_frame(Game_State_Maze *g)
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

typedef struct
{
    Pos p;
    // width and height
    Pos w_h;
} Box;

static void box_draw(Box *b, World_State0 *w)
{
    draw_blocks_warp(b->p, b->w_h, PINK, w);
}

static bool rect_intersection(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2)
{
    return r1.x < r2.x + w_h2.x && r1.x + w_h1.x > r2.x && r1.y < r2.y + w_h2.y && r1.y + w_h1.y > r2.y;
}
static bool rect_intersection_wrap(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2, const World_State0 *w)
{
    const Pos c1_x = {0, r1.y}, c1_x_wh = {r1.x + w_h1.x - w->width, w_h1.y};
    const Pos c1_y = {r1.x, 0}, c1_y_wh = {w_h1.x, r1.y + w_h1.y - w->height};
    const Pos c2_x = {0, r2.y}, c2_x_wh = {r2.x + w_h2.x - w->width, w_h2.y};
    const Pos c2_y = {r2.x, 0}, c2_y_wh = {w_h2.x, r2.y + w_h2.y - w->height};
    const Pos c1_xy = {0, 0}, c1_xy_wh = {r1.x + w_h1.x - w->width, r1.y + w_h1.y - w->height};
    const Pos c2_xy = {0, 0}, c2_xy_wh = {r2.x + w_h2.x - w->width, r2.y + w_h2.y - w->height};
    return rect_intersection(r1, w_h1, r2, w_h2) || // handle all cases, YES THIS IS NECESSARY
           rect_intersection(c1_y, c1_y_wh, r2, w_h2) || rect_intersection(c2_y, c2_y_wh, r1, w_h1) ||
           rect_intersection(c1_x, c1_x_wh, r2, w_h2) || rect_intersection(c2_x, c2_x_wh, r1, w_h1) ||
           rect_intersection(c1_xy, c1_xy_wh, r2, w_h2) || rect_intersection(c2_xy, c2_xy_wh, r1, w_h1) ||
           rect_intersection(c1_x, c1_x_wh, c2_y, c2_y_wh) || rect_intersection(c2_x, c2_x_wh, c1_y, c1_y_wh);
}

// For boxes, and their number, we have a position and a direction toghether with a width and height, that will move the
// boxes, excluding one index of boxes. Also world
static void boxes_player_collision_logic(Box boxes[], const Int nr_boxes, const Pos pos, const Dir dir, const Pos w_h,
                                         const Int exclude_idx, const World_State0 *w)
{
    for (Int box_idx = 0; box_idx < nr_boxes; ++box_idx)
    {
        if (box_idx == exclude_idx)
            continue;

        Box *box = &boxes[box_idx];

        if (rect_intersection_wrap(box->p, box->w_h, pos, w_h, w))
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
    g.w = world_state0_init(60);
    g.player = (Player){.length = 5, .idx_pos = 4, .current_direction = Dir_Right, .next_direction = Dir_Right};
    g.player.positions[4] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.player.positions[3] = (Pos){.x = g.w.width / 2 - 1, g.w.height / 2};
    g.player.positions[2] = (Pos){.x = g.w.width / 2 - 2, g.w.height / 2};
    g.player.positions[1] = (Pos){.x = g.w.width / 2 - 3, g.w.height / 2};
    g.player.positions[0] = (Pos){.x = g.w.width / 2 - 4, g.w.height / 2};

    food_init_position(&g.food, &g.player, &g.w);

    g.boxes[0] = (Box){.p = {1, 1}, .w_h = {4, 4}};
    g.boxes[1] = (Box){.p = {10, 5}, .w_h = {1, 1}};
    g.boxes[2] = (Box){.p = {40, 5}, .w_h = {12, 11}};
    g.boxes[3] = (Box){.p = {44, 19}, .w_h = {7, 6}};
    g.boxes[4] = (Box){.p = {36, 26}, .w_h = {6, 7}};
    g.boxes[5] = (Box){.p = {42, 35}, .w_h = {5, 9}};
    g.boxes[6] = (Box){.p = {50, 36}, .w_h = {6, 4}};
    g.boxes[7] = (Box){.p = {22, 30}, .w_h = {10, 10}};
    g.boxes[8] = (Box){.p = {18, 10}, .w_h = {14, 8}};
    g.boxes[9] = (Box){.p = {1, 10}, .w_h = {8, 14}};
    g.boxes[10] = (Box){.p = {10, 26}, .w_h = {6, 7}};
    g.boxes[11] = (Box){.p = {3, 30}, .w_h = {4, 3}};
    g.boxes[12] = (Box){.p = {2, 35}, .w_h = {3, 4}};
    g.boxes[13] = (Box){.p = {3, 41}, .w_h = {10, 2}};
    g.boxes[14] = (Box){.p = {15, 5}, .w_h = {10, 3}};
    g.boxes[15] = (Box){.p = {35, 5}, .w_h = {2, 10}};
    g.boxes[16] = (Box){.p = {55, 5}, .w_h = {4, 15}};
    g.boxes[17] = (Box){.p = {35, 20}, .w_h = {3, 3}};
    g.boxes[18] = (Box){.p = {14, 20}, .w_h = {6, 4}};
    g.boxes[19] = (Box){.p = {7, 3}, .w_h = {1, 2}};

    g.time_for_move = 1.0;

    *new_g = g;
}

// normal snake
static Level_Return game_state0_frame0(Game_State0 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

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
    player_set_direction_from_input(&g->player);

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

// gigantic free fast
static Level_Return game_state0_frame2(Game_State0 *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.02))
    {
        if (player_move(&g->player, w))
        {
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
    player_set_direction_from_input(&g->player);

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
            if (evil_snake_player_collision_logic(g->evil_snakes, &g->player))
                return Level_Return_Reset_Level;

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

static Level_Return game_state2_frame0(Game_State2 *g)
{
    // logic
    player_set_direction_from_input(&g->player);

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
    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.08))
    {
        if (player_move(&g->player, &g->w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }

        boxes_player_collision_logic(g->boxes, GAME_STATE4_BOXES, player_nth_position(&g->player, 0),
                                     g->player.current_direction, (Pos){1, 1}, -1, &g->w);

        food_player_collision_logic(&g->player, &g->food, &g->w);
    }
    Int food_left_to_win = (DEV ? 16 : 16) - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw_extra(&g->player, &g->w);
    food_draw(&g->food, &g->w);
    for (Int i = 0; i < GAME_STATE4_BOXES; i++)
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
        Int skip = 16;
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
    case 10: { // gigantic free fast
        mg.frame_code = (Meta_Game_Frame_Code)game_state0_frame2;
        mg.init_code = (Meta_Game_Init_Code)game_state0_init1;
        mg.data = malloc(sizeof(Game_State0));
    }
    break;
    case 11: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 12: { // ever growing
        mg.frame_code = (Meta_Game_Frame_Code)game_state4_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_state4_init;
        mg.data = malloc(sizeof(Game_State4));
    }
    break;
    case 13: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 14: { // You are food
        mg.frame_code = (Meta_Game_Frame_Code)game_state_YouFood_frame;
        mg.init_code = (Meta_Game_Init_Code)game_state_YouFood_init;
        mg.data = malloc(sizeof(Game_State_YouFood));
    }
    break;
    case 15: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
    }
    break;
    case 16: { // You are food
        mg.frame_code = (Meta_Game_Frame_Code)game_state_Maze_frame;
        mg.init_code = (Meta_Game_Init_Code)game_state_Maze_init;
        mg.data = malloc(sizeof(Game_State_Maze));
    }
    break;
    case 17: {
        mg.frame_code = (Meta_Game_Frame_Code)game_cutscene0_frame0;
        mg.init_code = (Meta_Game_Init_Code)game_cutscene0_init;
        mg.data = malloc(sizeof(Game_Cutscene0));
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
