#include "snake_pather.h"
#include <assert.h>

// Inits a snake_pather correctly, MAKE SURE WAY LENGTH ARE UNDER THE MAXIMUM
// DOES NOT INIT POSITION
Snake_Pather snake_pather_init_except_position(Snake_Pather_Way ways[], Int nr_ways)
{
	assert(nr_ways <= SNAKE_PATHER_MAX_WAYS);
    Snake_Pather pather = (Snake_Pather){
        .ways_len = nr_ways,
        .way_idx = nr_ways,
        .walk_this_way_counter = 0,
    };
    for (Int i = 0; i < nr_ways; ++i)
    {
        pather.ways[i] = ways[i];
    }
	return pather;
}

void snake_pather_draw(Snake_Pather *snake_pather, World_State0 *w)
{
    draw_snakelike(snake_pather->positions, snake_pather->len, SKYBLUE, BLUE, w);
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
    return player_intersection_points(player, snake_pather->positions, snake_pather->len);
}
