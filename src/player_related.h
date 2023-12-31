#pragma once
#include "very_general.h"

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

Pos player_nth_position(const Player *player, Int idx);
Player player_init(Pos start_pos, const Int length, const Dir dir);
// Cycle through positions of player
void player_draw_general(const Player *player, const Color head, const Color body, const World_State0 *w);
void player_draw(const Player *player, const World_State0 *w);
void player_draw_extra(const Player *player, const World_State0 *w);
void player_draw_green(const Player *player, const World_State0 *w);
void player_draw_flashing(const Player *player, const World_State0 *w);
void player_set_direction_correctly(Player *player, Dir dir);
void player_set_direction_from_input(Player *player);
// turns relative to previous direction
void player_set_direction_from_input_spinny(Player *player);
bool player_position_in_player(const Player *player, const Pos pos);
bool player_move(Player *player, const World_State0 *w);
bool player_intersection_point(const Player *player, const Pos point);
bool player_intersection_points(const Player *player, const Pos *positions, const Int length);

void food_draw(const Food *food, const World_State0 *w);
void food_draw_red(const Food *food, const World_State0 *w);
void food_set_random_position(Food *food, const World_State0 *w);
void food_init_position(Food *food, const Player *player, const World_State0 *w);

void food_player_collision_logic(Player *player, Food *food, const World_State0 *w);
void food_player_collision_logic_food_disappear(Player *player, Food *food);
