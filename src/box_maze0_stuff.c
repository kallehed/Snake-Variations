
#include "box_maze0_stuff.h"

// returns whether a box was stopped by a wall
bool boxes_collision_logic_maze0(Box boxes[], const Int nr_boxes, const Pos pos, const Dir dir, const Pos w_h,
                                 const Int exclude_idx, Maze0_Cell maze[], Int maze_width, const World_State0 *w)
{
    for (Int box_idx = 0; box_idx < nr_boxes; ++box_idx)
    {
        if (exclude_idx == box_idx)
            continue;

        Box *box = &boxes[box_idx];

        if (rect_intersection_wrap(box->p, box->w_h, pos, w_h, w))
        {
            Pos new_box_pos = move_inside_grid(box->p, dir, w);
            if (rect_intersect_maze0_wall(new_box_pos.x, new_box_pos.y, box->w_h.x, box->w_h.y, maze, maze_width))
            {
                return true;
            }
            else
            {
                Pos old_box_pos = box->p;
                box->p = new_box_pos;
                if (boxes_collision_logic_maze0(boxes, nr_boxes, box->p, dir, box->w_h, box_idx, maze, maze_width, w))
                {
                    box->p = old_box_pos; // propogate trueness upward, to disrupt any boxes that changed
                    return true;
                }
            }
        }
    }
    return false;
}
