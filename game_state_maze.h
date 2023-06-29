#pragma once

#include "very_general.h"
#include "player_related.h"
#include "maze_stuff.h"

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
    Maze0_Cell maze[GAME_STATE_MAZE_HEIGHT][GAME_STATE_MAZE_WIDTH];
} Game_State_Maze;

void game_state_Maze_init(Game_State_Maze *new_g);
Level_Return game_state_Maze_frame(Game_State_Maze *g);

void snake_pather_draw(Snake_Pather *snake_pather, World_State0 *w);
void snake_pather_move(Snake_Pather *snake_pather, World_State0 *w);
bool snake_pather_player_intersection(Snake_Pather *snake_pather, Player *player);
