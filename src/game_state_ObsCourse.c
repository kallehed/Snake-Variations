#include "game_state_ObsCourse.h"
#include "level_declarations.h"
#include "player_related.h"
#include "very_general.h"
#include <math.h>
#include <stddef.h>

void level_set_ObsCourse(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_frame_ObsCourse;
    mg->init_code = (Level_Init_Code)game_state_init_ObsCourse;
    mg->size = (sizeof(Game_State_ObsCourse));
}

void game_state_init_ObsCourse(Game_State_ObsCourse *new_g)
{
    Game_State_ObsCourse g;
    g.w = world_state0_init(20);
    g.w = world_state0_init_general(GS_OBSCOURSE_WIDTH, GS_OBSCOURSE_HEIGHT, WINDOW_HEIGHT / GS_OBSCOURSE_HEIGHT);
    g.player = player_init((Pos){4, 5}, GS_OBSCOURSE_PLAYER_START_LENGTH, Dir_Right);
    g.time_for_move = 1.0;

    g.cam_x = 0.f;

    Int i = 0;
    g.speed_points[i++] = (Speed_Point){.from_x = 0, .speed = 0.8f};
    g.speed_points[i++] = (Speed_Point){.from_x = 10, .speed = 2.0f};
    g.speed_points[i++] = (Speed_Point){.from_x = 20, .speed = 3.0f};
    g.speed_points[i++] = (Speed_Point){.from_x = 30, .speed = 3.5f};
    g.speed_points[i++] = (Speed_Point){.from_x = 100, .speed = 4.0f};
    g.speed_points[i++] = (Speed_Point){.from_x = 127, .speed = 15.f};
    g.speed_points[i++] = (Speed_Point){.from_x = 135, .speed = 0.5};
    g.speed_points[i++] = (Speed_Point){
        .from_x = 181,
        .speed = 0.f}; // negative speed points seem to slow down to 0 before even coming to the point, not goo

    // clang-format off
	//   Divided by 100 width block (currently only 2)																		 	 F        S
    const char *map[] = {
        "------------------------------------------------------------|||||||||||||||||||||||||||||||||-------""---||||----|------------------------------------------------|-|-----------------|-------------------",
        "----------------------------------------------------||||----||||--||||-------------------||||-------""---||||---|-|---------------|-------------|-----------------|-|-----------------|-------------------",
        "--------------------------------------------||||------------||||--||||------||||-||||----||||-------""---||||----|---------------|-|-----------|-|----------------|-|-----------------|-------------------",
        "------------------------||||-------||||-----||||----||||----||||------||||--|||||||||----||||-------""---||||---------------------|-------------|-----------------|-------------------|-------------------",
        "------------------------||||-------||||-------------||||--------------||||--|||||||||---||||--------""---||||-----------------------------------------------------|||----F------------|-------------------",
        "----------------|||---------------------------------------------------------|||||||||---||||--------""---||||||||||||||||||||||||||||||||||||||||||||||-------------|-----------------|-------------------",
        "----------------|||-----||||------------------------||||----||||-----||-----|||||||------||||-------""------------------------------------------------------------|-|-F---------------|-------------------",
        "------------------------||||------------------------||||----||||-----||--||-|||||||||----||||-------""---||||||||||||||||||||||||||||||||||||||||||||||-----------|-|-----------------|-------------------",
        "------------------------------------||||-----------------------------||--||-||||||||||||-||||-------""---||||-----------------------------------------------------|-|-----------------|-------------------",
        "------------------------------------||||------------||||-----------------||-||||||||||||-||||-------""---||||-------------|---------------------------------------|-|-------F---------|-------------------",
        "----------------------------------------------------||||----||||-------||---|||||||||----||||-------""---||||------------|-|-----|--------------|-----------------|-|-----------------|-------------------",
        "----------------------------||||----------------------------||||-------||-|||||||||||---|||||-------""---||||-------------|-----|-|------------|-|----------------|-|-----------------|-------------------",
        "----------------------------||||--------------------||||---------------||-||||||||||||--|||||-------""---||||-----|--------------|--------------|-----------------|-|---F-------------|-------------------",
        "----------------------------------------------------||||----||||----------||||||||||||--------------""---||||----|-|----------------------------------------------|-|-----------------|-------------------",
        "------------------------------------------------------------|||||||||||||||||||||||||---------------""---||||-----|-----------------------------------------------|-|-----------------|-------------------",
    };
    // clang-format on

    maze0_init_from_string(map, GS_OBSCOURSE_WIDTH, GS_OBSCOURSE_HEIGHT, (Food *)g.foods, (Maze0_Cell *)g.maze, NULL,
                           NULL, NULL, NULL, &g.w);

    *new_g = g;
}

// normal snake
Level_Return game_state_frame_ObsCourse(Game_State_ObsCourse *g)
{
    const World_State0 *const w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic(&g->time_for_move))
    {
        if (maze0_player_move((Maze0_Cell *)g->maze, GS_OBSCOURSE_WIDTH, &g->player, w))
        {
            // player died
            TraceLog(LOG_INFO, "%s", "YOU DIED!");

            return Level_Return_Reset_Level;
        }
        for (Int i = 0; i < GS_OBSCOURSE_MAX_FOODS; ++i)
            food_player_collision_logic_food_disappear(&g->player, &g->foods[i]);
    }

    // die by going offscreen
    {
        const Pos p_pos = player_nth_position(&g->player, 0);
        if ((p_pos.x + 1) * w->block_pixel_len < g->cam_x ||
            (p_pos.x - 1) * w->block_pixel_len > g->cam_x + WINDOW_WIDTH)
        {
            return Level_Return_Reset_Level;
        }
    }

    const Int food_left_to_win = GS_OBSCOURSE_MAX_FOODS + GS_OBSCOURSE_PLAYER_START_LENGTH - g->player.length;

    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, 10, WINDOW_HEIGHT, RED);
    DrawRectangle(WINDOW_WIDTH - 10, 0, 10, WINDOW_HEIGHT, RED);
    draw_fps();
    {
        // TraceLog(LOG_INFO, "cam_x: %f", cam_x); // 2*width/(3)  width/ 2
        const Coord cam_x_coord = g->cam_x / w->block_pixel_len;
        for (Int i = 1; i < GS_OBSCOURSE_MAX_SPEED_POINTS; ++i)
        {
            if (cam_x_coord < g->speed_points[i].from_x)
            {
                const Int are = i - 1;
                const Int next = i;
                const Coord are_coord = g->speed_points[are].from_x;
                const Coord next_coord = g->speed_points[next].from_x;
                const float speed_are = g->speed_points[are].speed;
                const float speed_next = g->speed_points[next].speed;
                const float speed =
                    speed_are + (speed_next - speed_are) * (cam_x_coord - are_coord) / (next_coord - are_coord);
                // found the speed point at which we are
                g->cam_x += speed * GetFrameTime() * 60.f;
                break;
            }
        }
        BeginMode2D((Camera2D){
            .offset = {.x = 0.f, .y = 0.f}, .target = {.x = g->cam_x, .y = 0.f}, .rotation = 0.f, .zoom = 1.f});
    }
    maze0_draw((Maze0_Cell *)g->maze, GS_OBSCOURSE_WIDTH, GS_OBSCOURSE_HEIGHT, w);
    {
        Int wid = WINDOW_WIDTH * 2;
        draw_food_left_general(food_left_to_win, wid * (((Int)(g->cam_x + WINDOW_WIDTH)) / wid), -40);
    }

    player_draw(&g->player, w);
    for (Int i = 0; i < GS_OBSCOURSE_MAX_FOODS; ++i)
    {
        food_draw(&g->foods[i], w);
    }

    EndDrawing();
    return Level_Return_Continue;
}
