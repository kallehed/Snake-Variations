#include "maze_stuff.h"
#include "snake_pather.h"
#include "very_general.h"
#include <assert.h>

void maze0_init_from_string(const char *const *const map, const Int width, const Int height, Food foods[],
                            Maze0_Cell maze[], Snake_Pather pathers[], const Int pather_lengths[],
                            const Dir pather_dirs[], Box boxes[], const World_State0 *w)
{
    Int food_idx = 0;
    Int pather_idx = 0;
    Int box_idx = 0;
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
            case 'S': {
                Snake_Pather *pather = &pathers[pather_idx];
                const Int pather_length = pather_lengths[pather_idx];
                const Dir pather_dir = pather_dirs[pather_idx];

                assert(pather_length <= SNAKE_PATHER_MAX_LEN);
                pather->len = pather_length;

                Pos pather_pos = (Pos){j, i};
                for (Int k = 0; k < pather_length; ++k)
                {
                    pather->positions[k] = pather_pos;
                    pather_pos = move_inside_grid(pather_pos, pather_dir, w);
                }

                pather_idx++;
            }
            break;
            case 'B': {
                boxes[box_idx++].p = (Pos){j, i};
            }
            break;
            case 'O': {
                cell = Maze0_Cell_Button;
            }
            break;
            case 'w': { 
                cell = Maze0_Cell_Water;
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
            Color col = {0,255,255,255};
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
            case Maze0_Cell_Button: {
                col = (Color){0, 235, 20, 255};
            }
            break;
            case Maze0_Cell_Water: {
                col = (Color){10, 30, 242, 255};
            }
            break;
            }
            draw_block_at((Pos){j, i}, col, w);
        }
    }
}

// PURE, says whether the player can actually move
bool maze0_player_can_move(const Maze0_Cell maze[], const Int maze_width, const Player *const player,
                           const World_State0 *const w)
{
    Pos next_pos = move_inside_grid(player_nth_position(player, 0), player->next_direction, w);
    return maze[next_pos.y * maze_width + next_pos.x] != Maze0_Cell_Wall;
}

// returns whether player died going into self or not
bool maze0_player_move(const Maze0_Cell maze[], const Int maze_width, Player *player, const World_State0 *w)
{
    if (maze0_player_can_move(maze, maze_width, player, w))
    {
        return player_move(player, w);
    }
    return false;
}

bool rect_intersect_maze0_wall(const Int r_x, const Int r_y, const Int r_width, const Int r_height,
                               const Maze0_Cell maze[], const Int maze_width)
{
    for (Int y = r_y; y < r_y + r_height; ++y)
    {
        for (Int x = r_x; x < r_x + r_width; ++x)
        {
            Int new_x = x;
            if (new_x >= maze_width) new_x -= maze_width;
            if (Maze0_Cell_Wall == maze[y * maze_width + new_x])
            {
                return true;
            }
        }
    }
    return false;
}
