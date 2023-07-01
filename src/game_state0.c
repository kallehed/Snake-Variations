#include "game_state0.h"
#include "player_related.h"
#include "very_general.h"
#include <stdlib.h>

void metagame_set_level_First(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state0_frame0;
    mg->init_code = (Meta_Game_Init_Code)game_state0_init0;
    mg->size = (sizeof(Game_State0));
}

void metagame_set_level_Skin(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state0_frame1;
    mg->init_code = (Meta_Game_Init_Code)game_state0_init0;
    mg->size = (sizeof(Game_State0));
}

void metagame_set_level_GigFreeFast(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state0_frame2;
    mg->init_code = (Meta_Game_Init_Code)game_state0_init1;
    mg->size = (sizeof(Game_State0));
}
void metagame_set_level_GetSmall(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state0_frameGetSmall;
    mg->init_code = (Meta_Game_Init_Code)game_state0_init_GetSmall;
    mg->size = (sizeof(Game_State0));
}
void metagame_set_level_Spinny(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state0_frame_Spinny;
    mg->init_code = (Meta_Game_Init_Code)game_state0_init0;
    mg->size = (sizeof(Game_State0));
}

void metagame_set_level_Wait(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_Wait;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_Wait;
    mg->size = (sizeof(Game_State_Wait));
}

void game_state0_init0(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(12);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

// Gigantic free fast
void game_state0_init1(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(120);
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    food_init_position(&g.food, &g.player, &g.w);
    g.time_for_move = 1.0;

    *new_g = g;
}

void game_state0_init_GetSmall(Game_State0 *new_g)
{
    Game_State0 g;
    g.w = world_state0_init(12);
    g.player = (Player){.length = 12, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
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
    g.player = (Player){.length = 1, .idx_pos = 0, .current_direction = Dir_Nothing, .next_direction = Dir_Right};
    g.player.positions[0] = (Pos){.x = g.w.width / 2, g.w.height / 2};
    g.warps_done = 0;
    g.time_started = GetTime();
    g.time_for_move = 1.0;

    *new_g = g;
}

// normal snake
Level_Return game_state0_frame0(Game_State0 *g)
{
    World_State0 *w = &g->w;
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

    Int food_left_to_win = (DEV ? 2 : 6) - g->player.length;

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
    World_State0 *w = &g->w;
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

    Int food_left_to_win = (DEV ? 2 : 12) - g->player.length;
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
    World_State0 *w = &g->w;
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
    World_State0 *w = &g->w;
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

    Int food_left_to_win = (DEV ? 8 : 8) - g->player.length;
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
    World_State0 *w = &g->w;
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

    Int food_left_to_win = (DEV ? 10 : 10) - g->player.length;
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
    World_State0 *w = &g->w;
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
