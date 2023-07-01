#pragma once
#include "very_general.h"
#include "player_related.h"

#define SEEKER_MAX_LENGTH 30
typedef struct
{
    Int length;
    Dir direction;
    Pos positions[SEEKER_MAX_LENGTH];
} Seeker_Snake;

void seeker_draw_general(const Seeker_Snake *snake, Color head, Color body, const World_State0 *w);
void seeker_move(Seeker_Snake *snake, World_State0 *w);
void seeker_draw(const Seeker_Snake *snake, const World_State0 *w);
// returns true on collision
bool seeker_player_collision_logic(const Seeker_Snake *snake, const Player *player);
