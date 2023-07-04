#pragma once
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>

#define DEV 1
// Whether to cycle through all levels quickly at beginning, to test their validity
// #define TEST_ALL_LEVELS
#define GAME_FPS 60

#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT (630)

typedef int16_t Coord; // Coord in world

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

typedef struct
{
    Dir dir;
    Pos pos;
} Dir_And_Pos;

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

typedef Level_Return (*Meta_Game_Frame_Code)(void *);
typedef void (*Meta_Game_Init_Code)(void *);

typedef struct
{
    Meta_Game_Frame_Code frame_code;
    Meta_Game_Init_Code init_code;
    uint32_t size;

    void *_data; // should not be touched by anything other than main.c
    Int frame;
} Meta_Game;

// for setting function pointers to init code, frame code, and mallocing the data
typedef void (*Meta_Game_Set_Level_Code)(Meta_Game *);

bool rect_intersection(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2);
bool rect_intersection_wrap(const Pos r1, const Pos w_h1, const Pos r2, const Pos w_h2, const World_State0 *w);
// assumes length >= 1
Int smallest_index_of_numbers(const Int numbers[], const Int length);

Pos dir_to_pos(Dir d);
Dir dir_turn_clockwise(Dir d);
Dir dir_turn_counter_clockwise(Dir d);
Dir dir_opposite(const Dir d);

bool pos_equal(Pos p, Pos q);
// simply moves a position without any care for warping
Pos pos_move(Pos pos, const Dir dir);

Pos move_inside_grid(Pos pos, const Dir dir, const World_State0 *w);
// useful for setting a snakes positions from a start using a direction
void set_positions_as_line_from(Pos positions[], const Int length, Pos start, const Dir dir, const World_State0 *w);
void set_positions_as_line_from_without_wrapping(Pos positions[], const Int length, Pos start, const Dir dir);
// useful for spawning things outside the game
Dir_And_Pos random_outside_edge_position_and_normal(const World_State0 *w);

void draw_block_at(Pos pos, Color color, const World_State0 *w);
void draw_blocks_at(Pos pos, Pos w_h, Color color, const World_State0 *w);
// draw warping, assuming it can not have negative position(ex: boxes) and will only overflow at width and height of
// world
void draw_blocks_warp(Pos pos, Pos w_h, Color color, const World_State0 *w);
void draw_snakelike(Pos positions[], Int length, Color head, Color body, const World_State0 *w);

World_State0 world_state0_init(Int width);
// For when you want a scrollable world
World_State0 world_state0_init_general(Int width, Int height, Int block_pixel_len);

// In pixels
void draw_food_left_in_2D_space_general(Int food_left_to_win, Int width, Int height, Int offset_x, Int offset_y);
void draw_food_left_in_2D_space(Int food_left_to_win, Int width, Int height);
void draw_food_left_general(Int food_left_to_win, int x, int y);
void draw_food_left(Int food_left_to_win);

void draw_fps(void);
bool time_move_logic_general(double *time_for_move, const double wait_time);
bool time_move_logic(double *time_for_move);
// NOT PURE
Dir get_dir_from_input(void);
