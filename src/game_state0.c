#include "game_state0.h"
#include "level_declarations.h"
#include "player_related.h"
#include "very_general.h"
#include <stdlib.h>

void level_set_First(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state0_frame0;
    mg->init_code = (Level_Init_Code)game_state0_init0;
    mg->size = (sizeof(Game_State0));
}

void level_set_Skin(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state0_frame1;
    mg->init_code = (Level_Init_Code)game_state0_init0;
    mg->size = (sizeof(Game_State0));
}

void level_set_GigFreeFast(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state0_frame2;
    mg->init_code = (Level_Init_Code)game_state0_init1;
    mg->size = (sizeof(Game_State0));
}
void level_set_GetSmall(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state0_frameGetSmall;
    mg->init_code = (Level_Init_Code)game_state0_init_GetSmall;
    mg->size = (sizeof(Game_State0));
}
void level_set_Spinny(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state0_frame_Spinny;
    mg->init_code = (Level_Init_Code)game_state0_init0;
    mg->size = (sizeof(Game_State0));
}

void level_set_Wait(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_frame_Wait;
    mg->init_code = (Level_Init_Code)game_state_init_Wait;
    mg->size = (sizeof(Game_State_Wait));
}

void game_state0_init0(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(12);
    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 1, Dir_Right);
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

// Gigantic free fast
void game_state0_init1(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(120);
    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 1, Dir_Right);
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

void game_state0_init_GetSmall(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(12);
    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 12, Dir_Right);
    for (Int i = 1; i < PLAYER_MAX_POSITIONS; ++i)
    {
        g.player.positions[i] = (Pos){.x = -1, .y = -1};
    }
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

void game_state_init_Wait(Game_State_Wait *new_g)
{
    Game_State_Wait g;
    g.w = world_state0_init(12);
    g.player = player_init((Pos){.x = g.w.width / 2, g.w.height / 2}, 1, Dir_Right);
    g.warps_done = 0;
    g.time_started = GetTime();
    g.time_for_move = 1.0;

    *new_g = g;
}

// normal snake
Level_Return game_state0_frame0(Game_State0 *g)
{
    const World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = 7 - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    player_draw(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

// flashier snake
Level_Return game_state0_frame1(Game_State0 *g)
{
    const World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = 12 - g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

Level_Return game_state0_frameGetSmall(Game_State0 *g)
{
    const World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        if (pos_equal(player_nth_position(&g->player, 0), g->food.pos))
        {
            --g->player.length;
            food_init_position(&g->food, &g->player, w);
        }
    }

    Int food_left_to_win = g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
// gigantic free fast
Level_Return game_state0_frame2(Game_State0 *g)
{
    const World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.02))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = 8 - g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
// Spinny
Level_Return game_state0_frame_Spinny(Game_State0 *g)
{
    const World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input_spinny(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (player_move(&g->player, w))
        {
            return Level_Return_Reset_Level;
        }
        food_player_collision_logic(&g->player, &g->food, w);
    }

    Int food_left_to_win = 10 - g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);

    player_draw_extra(&g->player, w);
    food_draw(&g->food, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}

Level_Return game_state_frame_Wait(Game_State_Wait *g)
{
    const World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        Pos start = player_nth_position(&g->player, 0);
        if (player_move(&g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }
        Pos end = player_nth_position(&g->player, 0);

        Int dist = abs(start.x - end.x + start.y - end.y);
        if (dist > 1)
        {
            g->warps_done++;
        }
    }
    Int food_left_to_win = 12 + g->warps_done - (Int)(GetTime() - g->time_started);
    // TraceLog(LOG_INFO, "Waiting level data: warps: %d, time_started: %f, cur_time: %f, food_left: %d", g->warps_done,
    // g->time_started, GetTime(), food_left_to_win);

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_food_left(food_left_to_win);

    player_draw(&g->player, w);

    draw_fps();
    EndDrawing();
    return Level_Return_Continue;
}
