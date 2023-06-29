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
#define SNAKE_PATHER_MAX_LEN 30
typedef struct
{
    Pos positions[SNAKE_PATHER_MAX_LEN];
    int8_t len;
    // Ways to go in path
    Snake_Pather_Way ways[SNAKE_PATHER_MAX_WAYS];
    // CONST. how many actual ways you have
    int8_t ways_len;
    // What way are we currently on ^
    int8_t way_idx;
    // Starts high, goes to zero, depending on Way::len
    int8_t walk_this_way_counter;
} Snake_Pather;

// Inits a snake_pather correctly, MAKE SURE LENGTH AND WAY LENGTH ARE UNDER THE MAXIMUM
Snake_Pather snake_pather_init(Pos positions[], Int length, Snake_Pather_Way ways[], Int nr_ways);

void snake_pather_draw(Snake_Pather *snake_pather, World_State0 *w);
void snake_pather_move(Snake_Pather *snake_pather, World_State0 *w);
bool snake_pather_player_intersection(Snake_Pather *snake_pather, Player *player);
