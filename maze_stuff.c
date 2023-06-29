#include "maze_stuff.h"
#include "very_general.h"

void maze0_init_from_string(const char **const map, const Int width, const Int height, Food foods[], Maze0_Cell maze[])
{
    Int food_idx = 0;
    for (Int i = 0; i < height; ++i)
    {
        for (Int j = 0; j < width; ++j)
        {
            Maze0_Cell cell = Maze0_Cell_Empty;
            switch (map[i][j])
            {
            case '|': {
                cell = Maze0_Cell_Wall;
            }
            break;
            case 'F': {
                foods[food_idx++] = (Food){.pos = {.x = j, .y = i}};
            }
            break;
            default: {
                cell = Maze0_Cell_Empty;
            }
            break;
            }
            Int idx = i * width + j;
            maze[idx] = cell;
        }
    }
}

void maze0_draw(const Maze0_Cell maze[], const Int maze_width, const Int maze_height, const World_State0 *w)
{
    for (Int i = 0; i < maze_height; ++i)
    {
        for (Int j = 0; j < maze_width; ++j)
        {
            Maze0_Cell cell = maze[i * maze_width + j];
            Color col;
            switch (cell)
            {
            case Maze0_Cell_Empty: {
                continue;
            }
            break;
            case Maze0_Cell_Wall: {
                col = (Color){0, 0, 0, 200};
            }
            break;
            }
            draw_block_at((Pos){j, i}, col, w);
        }
    }
}

// returns whether player died going into self or not
bool maze0_player_move(Maze0_Cell maze[], const Int maze_width, Player *player, World_State0 *w)
{
    Pos next_pos = move_inside_grid(player_nth_position(player, 0), player->next_direction, w);
    if (maze[next_pos.y * maze_width + next_pos.x] != Maze0_Cell_Wall) // check that the next position is not in a wall
    {
        return player_move(player, w);
    }
    return false;
}
