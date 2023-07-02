#pragma once

#include "box_general.h"
#include "maze_stuff.h"
#include "very_general.h"

// returns whether a box was stopped by a wall
bool boxes_collision_logic_maze0(Box boxes[], const Int nr_boxes, const Pos pos, const Dir dir, const Pos w_h,
                                 const Int exclude_idx, Maze0_Cell maze[], Int maze_width, const World_State0 *w);
