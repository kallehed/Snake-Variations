#include "player_related.h"

Pos player_nth_position(const Player *player, Int idx)
{
    Int i = player->idx_pos - idx;
    while (i < 0)
        i += PLAYER_MAX_POSITIONS;
    return player->positions[i];
}

// Cycle through positions of player
void player_draw(const Player *player, const World_State0 *w)
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

void player_draw_extra(const Player *player, const World_State0 *w)
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
// do not turn in a way that instantly kills youself, handles next and current direction
void player_set_direction_correctly(Player *player, Dir dir)
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

void player_set_direction_from_input(Player *player)
{
    player_set_direction_correctly(player, get_dir_from_input());
}

void player_set_direction_from_input_spinny(Player *player)
{
	switch (get_dir_from_input()) {
		case Dir_Right:
			{
				player_set_direction_correctly(player, dir_turn_clockwise(player->current_direction));

		}
		break;
		case Dir_Left:
			{
				player_set_direction_correctly(player, dir_turn_counter_clockwise(player->current_direction));

		}
		break;
		default: {

		}
		break;
	}
}

bool player_position_in_player(const Player *player, const Pos pos)
{
    for (Int i = 0; i < player->length; ++i)
    {
        if (pos_equal(player_nth_position(player, i), pos))
        {
            return true;
        }
    }
    return false;
}

// returns true if player dies
bool player_move(Player *player, World_State0 *w)
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

bool player_intersection_point(const Player *player, const Pos point)
{
    for (Int i = 0; i < player->length; ++i)
    {
        Pos p_pos = player_nth_position(player, i);
        if (pos_equal(p_pos, point))
            return true;
    }
    return false;
}

void food_draw(const Food *food, const World_State0 *w)
{
    draw_block_at(food->pos, GREEN, w);
}
void food_set_random_position(Food *food, const World_State0 *w)
{
    Coord x = GetRandomValue(0, w->width - 1);
    Coord y = GetRandomValue(0, w->height - 1);
    food->pos.x = x;
    food->pos.y = y;
}
void food_init_position(Food *food, const Player *player, const World_State0 *w)
{
    do
    {
        food_set_random_position(food, w);
    } while (player_intersection_point(player, food->pos));
}
void food_player_collision_logic(Player *player, Food *food, const World_State0 *w)
{
    if (pos_equal(player_nth_position(player, 0), food->pos))
    {
        ++player->length;
        food_init_position(food, player, w);
    }
}

void food_player_collision_logic_food_disappear(Player *player, Food *food)
{
    if (pos_equal(player_nth_position(player, 0), food->pos))
    {
        ++player->length;
        food->pos = (Pos){.x = -1, .y = -1};
    }
}
