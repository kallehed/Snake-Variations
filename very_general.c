#include "very_general.h"
#include <stdio.h>
#include <stdlib.h>

bool rect_intersection(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2)
{
    return r1.x < r2.x + w_h2.x && r1.x + w_h1.x > r2.x && r1.y < r2.y + w_h2.y && r1.y + w_h1.y > r2.y;
}

bool rect_intersection_wrap(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2, const World_State0 *w)
{
    const Pos c1_x = {0, r1.y}, c1_x_wh = {r1.x + w_h1.x - w->width, w_h1.y};
    const Pos c1_y = {r1.x, 0}, c1_y_wh = {w_h1.x, r1.y + w_h1.y - w->height};
    const Pos c2_x = {0, r2.y}, c2_x_wh = {r2.x + w_h2.x - w->width, w_h2.y};
    const Pos c2_y = {r2.x, 0}, c2_y_wh = {w_h2.x, r2.y + w_h2.y - w->height};
    const Pos c1_xy = {0, 0}, c1_xy_wh = {r1.x + w_h1.x - w->width, r1.y + w_h1.y - w->height};
    const Pos c2_xy = {0, 0}, c2_xy_wh = {r2.x + w_h2.x - w->width, r2.y + w_h2.y - w->height};
    return rect_intersection(r1, w_h1, r2, w_h2) || // handle all cases, YES THIS IS NECESSARY
           rect_intersection(c1_y, c1_y_wh, r2, w_h2) || rect_intersection(c2_y, c2_y_wh, r1, w_h1) ||
           rect_intersection(c1_x, c1_x_wh, r2, w_h2) || rect_intersection(c2_x, c2_x_wh, r1, w_h1) ||
           rect_intersection(c1_xy, c1_xy_wh, r2, w_h2) || rect_intersection(c2_xy, c2_xy_wh, r1, w_h1) ||
           rect_intersection(c1_x, c1_x_wh, c2_y, c2_y_wh) || rect_intersection(c2_x, c2_x_wh, c1_y, c1_y_wh);
}
Pos dir_to_pos(Dir d)
{
    switch (d)
    {
    case Dir_Right:
        return (Pos){1, 0};
    case Dir_Left:
        return (Pos){-1, 0};
    case Dir_Up:
        return (Pos){0, -1};
    case Dir_Down:
        return (Pos){0, 1};
    case Dir_Nothing:
        return (Pos){0, 0};
    }
    abort();
}

Dir dir_turn_clockwise(Dir d)
{
    switch (d)
    {
    case Dir_Right:
        return Dir_Down;
    case Dir_Down:
        return Dir_Left;
    case Dir_Left:
        return Dir_Up;
    case Dir_Up:
        return Dir_Right;
    case Dir_Nothing:
        return Dir_Nothing;
    }
}

Dir dir_turn_counter_clockwise(Dir d)
{
    switch (d)
    {
    case Dir_Right:
        return Dir_Up;
    case Dir_Down:
        return Dir_Right;
    case Dir_Left:
        return Dir_Down;
    case Dir_Up:
        return Dir_Left;
    case Dir_Nothing:
        return Dir_Nothing;
    }
}

bool pos_equal(Pos p, Pos q)
{
    return p.x == q.x && p.y == q.y;
}

Pos move_inside_grid(Pos pos, const Dir dir, const World_State0 *w)
{
    switch (dir)
    {
    case Dir_Right:
        ++pos.x;
        if (pos.x >= w->width)
            pos.x = 0;
        break;
    case Dir_Left:
        --pos.x;
        if (pos.x < 0)
            pos.x = w->width - 1;
        break;
    case Dir_Up:
        --pos.y;
        if (pos.y < 0)
            pos.y = w->height - 1;
        break;
    case Dir_Down:
        ++pos.y;
        if (pos.y >= w->height)
            pos.y = 0;
        break;
    case Dir_Nothing:
        break;
    }
    return pos;
}
void draw_block_at(Pos pos, Color color, const World_State0 *w)
{
    DrawRectangle(pos.x * w->block_pixel_len, pos.y * w->block_pixel_len, w->block_pixel_len, w->block_pixel_len,
                  color);
}

// will not warp
void draw_blocks_at(Pos pos, Pos w_h, Color color, const World_State0 *w)
{
    DrawRectangle(pos.x * w->block_pixel_len, pos.y * w->block_pixel_len, w->block_pixel_len * w_h.x,
                  w->block_pixel_len * w_h.y, color);
}

// draw warping, assuming it can not have negative position(ex: boxes) and will only overflow at width and height of
// world
void draw_blocks_warp(Pos pos, Pos w_h, Color color, const World_State0 *w)
{
    draw_blocks_at(pos, w_h, color, w);
    Coord x_over = pos.x + w_h.x - w->width;
    Coord y_over = pos.y + w_h.y - w->height;
    if (x_over > 0)
    {
        draw_blocks_at((Pos){0, pos.y}, (Pos){x_over, w_h.y}, color, w);
    }
    if (y_over > 0)
    {
        draw_blocks_at((Pos){pos.x, 0}, (Pos){w_h.x, y_over}, color, w);
    }
    if (x_over > 0 && y_over > 0) // special case for when at width and height
    {
        draw_blocks_at((Pos){0, 0}, (Pos){x_over, y_over}, color, w);
    }
}

void draw_snakelike(Pos positions[], Int length, Color head, Color body, const World_State0 *w)
{
    for (Int i = 0; i < length; ++i)
    {
        draw_block_at(positions[i], (i == 0) ? head : body, w);
    }
}

World_State0 world_state0_init(Int width)
{
    World_State0 w = {.width = width};
    w.block_pixel_len = WINDOW_WIDTH / w.width;
    w.height = WINDOW_HEIGHT / w.block_pixel_len;
    return w;
}

// For when you want a scrollable world
World_State0 world_state0_init_general(Int width, Int height, Int block_pixel_len)
{
    World_State0 w = {.width = width, .height = height, .block_pixel_len = block_pixel_len};
    return w;
}

// In pixels
void draw_food_left_in_2D_space(Int food_left_to_win, Int width, Int height)
{
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d", food_left_to_win);
    for (Int i = 0; i < height; i += WINDOW_HEIGHT * 1.25)
    {
        for (Int j = 0; j < width; j += WINDOW_WIDTH)
        {
            DrawText(buffer, 200 + j, -40 + i, 800, (Color){0, 0, 0, 40});
        }
    }
}

void draw_food_left_general(Int food_left_to_win, int x, int y)
{
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d", food_left_to_win);
    DrawText(buffer, x, y, 800, (Color){0, 0, 0, 40});
}

void draw_food_left(Int food_left_to_win)
{
    draw_food_left_general(food_left_to_win, 200, -40);
}
void draw_fps(void)
{
    char myText[100];
    int fps = GetFPS();
    snprintf(myText, sizeof(myText), "FPS: %d", fps);
    DrawText(myText, 10, 10, 20, LIGHTGRAY);
}
bool time_move_logic_general(double *time_for_move, const double wait_time)
{
    double time = GetTime();
    if (time >= *time_for_move)
    {
        *time_for_move = time + wait_time;
        return true;
    }
    return false;
}

bool time_move_logic(double *time_for_move)
{
    return time_move_logic_general(time_for_move, 0.1);
}
// NOT PURE
Dir get_dir_from_input(void)
{
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_L))
    {
        return Dir_Right;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_H))
    {
        return Dir_Left;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_K))
    {
        return Dir_Up;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_J))
    {
        return Dir_Down;
    }
    return Dir_Nothing;
}
