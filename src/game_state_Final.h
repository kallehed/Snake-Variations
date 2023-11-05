#pragma once

#include "player_related.h"
#include "very_general.h"

typedef struct
{
    World_State0 w;
    Player *player;
    Food food;
    double time_for_move;
} GS_Final;

void GS_init_Final(GS_Final *new_g, Allo *allo, Sound sounds[]);
Level_Return GS_frame_Final(GS_Final *g);
