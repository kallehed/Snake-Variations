#include "seeker_snake.h"

void seeker_move(Seeker_Snake *snake, World_State0 *w)
{
    for (int i = snake->length - 1; i >= 1; --i)
    {
        snake->positions[i] = snake->positions[i - 1];
    }
    snake->positions[0] = move_inside_grid(snake->positions[0], snake->direction, w);
}

void seeker_draw_general(const Seeker_Snake *snake, Color head, Color body, const World_State0 *w)
{
    const Int length = snake->length;
    for (Int j = 0; j < length; ++j)
    {
        draw_block_at(snake->positions[j], (j == 0) ? head : body, w);
    }
}
void seeker_draw(const Seeker_Snake *snake, const World_State0 *w)
{
    seeker_draw_general(snake, SKYBLUE, BLUE, w);
}

// returns true on collision
bool seeker_player_collision_logic(const Seeker_Snake *snake, const Player *player)
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
