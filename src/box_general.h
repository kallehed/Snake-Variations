#pragma once
#include "very_general.h"

typedef struct
{
    Pos p;
    // width and height
    Pos w_h;
} Box;

void boxes_collision_logic(Box boxes[], const Int nr_boxes, const Pos pos, const Dir dir, const Pos w_h,
                                  const Int exclude_idx, const World_State0 *w);
void box_draw(Box *b, World_State0 *w);
