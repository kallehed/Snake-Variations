
#include "box_general.h"
#include "raylib.h"
#include "very_general.h"

// For boxes, and their number, we have a position and a direction toghether with a width and height, that will move the
// boxes, excluding one index of boxes. Also world
void boxes_collision_logic(Box boxes[], const Int nr_boxes, const Pos pos, const Dir dir, const Pos w_h,
                           const Int exclude_idx, const World_State0 *w)
{
    for (Int box_idx = 0; box_idx < nr_boxes; ++box_idx)
    {
        if (box_idx == exclude_idx)
            continue;

        Box *box = &boxes[box_idx];

        if (rect_intersection_wrap(box->p, box->w_h, pos, w_h, w))
        {
            box->p = move_inside_grid(box->p, dir, w);
            boxes_collision_logic(boxes, nr_boxes, box->p, dir, box->w_h, box_idx, w);
            // break here?
        }
    }
}

void box_draw(Box *b, World_State0 *w)
{
    draw_blocks_warp(b->p, b->w_h, PINK, w);
}
