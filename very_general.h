#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"

#define DEV 1
#define GAME_FPS 144

#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT (630)

typedef int8_t Coord; // Coord in world

typedef int32_t Int; // for general things like indexing arrays and stuff

typedef struct Pos
{
    Coord x, y;
} Pos;

typedef enum Dir
{
    Dir_Right = 0, // obligatory numbers for first 4, as YouFood level generates random dir
    Dir_Left = 1,
    Dir_Up = 2,
    Dir_Down = 3,
    Dir_Nothing,
} Dir;


typedef struct World_State0
{
    // Cells in current world
    Coord width, height;
    Int block_pixel_len;
} World_State0;

typedef enum Level_Return
{
    Level_Return_Continue = 0,
    Level_Return_Next_Level,
    Level_Return_Reset_Level,
} Level_Return;

bool rect_intersection(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2);
bool rect_intersection_wrap(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2, const World_State0 *w);	
Pos dir_to_pos(Dir d);
bool pos_equal(Pos p, Pos q);
Pos move_inside_grid(Pos pos, const Dir dir, const World_State0 *w);

void draw_block_at(Pos pos, Color color, const World_State0 *w);
void draw_blocks_at(Pos pos, Pos w_h, Color color, const World_State0 *w);
// draw warping, assuming it can not have negative position(ex: boxes) and will only overflow at width and height of
// world
void draw_blocks_warp(Pos pos, Pos w_h, Color color, const World_State0 *w);

World_State0 world_state0_init(Int width);
void draw_food_left(Int food_left_to_win);
void draw_fps(void);
bool time_move_logic_general(double *time_for_move, const double wait_time);
bool time_move_logic(double *time_for_move);
// NOT PURE
Dir get_dir_from_input(void);
