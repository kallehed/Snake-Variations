#include "game_state_Zelda.h"
#include "box_general.h"
#include "box_maze0_stuff.h"
#include "maze_stuff.h"
#include "player_related.h"
#include "raylib.h"
#include "snake_pather.h"
#include "very_general.h"
#include "level_declarations.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void level_set_Zelda(Level *mg)
{
    mg->frame_code = (Level_Frame_Code)game_state_frame_Zelda;
    mg->init_code = (Level_Init_Code)game_state_init_Zelda;
    mg->size = (sizeof(GS_Zelda));
}

void game_state_init_Zelda(GS_Zelda *new_g)
{
    GS_Zelda g;
    g.w = world_state0_init_general(GS_ZELDA_WIDTH, GS_ZELDA_HEIGHT, WINDOW_WIDTH / GS_ZELDA_ROOM_WIDTH);
    g.player = player_init((Pos){.x = GS_ZELDA_ROOM_WIDTH * 2.5, .y = GS_ZELDA_ROOM_HEIGHT * 1.5}, 8, Dir_Right);
    g.player_inv_timer = 0.f;
    g.room_x = -1;
    g.room_y = -1;
    g.room_change_timer = 0.f;
    g.time_for_move = 0.0;

    const char *map[] = {
        // clang-format off
       //0                   1                   2                   3                   4
        "------------------------------------------------------------|||||||||||||||||||||-------------------", // 0
        "------------------------------------------------------------|||||||||||||||||||||-------------------",
        "------------------------------------------------------------|||||||||||||||||||||-------------------",
        "------------------------------------------------------------|--||||||||||||||--||-------------------",
        "------------------------------------------------------------|---||||||||||||---||-------------------",
        "------------------------------------------------------------|---------|||------||-------------------",
        "------------------------------------------------------------||---------O-------||-------------------",
        "------------------------------------------------------------||-----------------||-------------------",
        "------------------------------------------------------------||----------------|||-------------------",
        "------------------------------------------------------------||----------------|||-------------------",
        "------------------------------------------------------------||---------------||||-------------------",
        "------------------------------------------------------------||--------------|||||-------------------",
        "------------------------------------------------------------|||-----------|||||||-------------------",
        "------------------------------------------------------------||||---------||||||||-------------------",
        "------------------------------------------------------------|||||-------|||||||||-------------------",
        "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||-||||||||||||||||||||||||||||", // 1
        "|S-----------------------------------|||||||-----------||||||||||||||||-|||||||||||O-O-O-O-O-O-O-O-|",
        "|------------------|||||||--||-|||||--|||||--|-|-|-|-|-||||||||||||||||-|||||||||||B---------------|",
        "|--------------------------------------||--------OO------||||||||||||||-|||||||||||--B-----------B-|",
        "|------------------|||||||--||-|||||----|--------OO-------|||||||||||||-|||||||||||----B-------B---|",
        "|----------------------------------------------OO--OO------||||||||||||-|||||||||--------B---B-----|",
        "|------------------|||||||--||-|||||-----------OO--OO-------|||||||||||-|||||||||----------B-------|",
        "|------------------------------------------------------------||||||||||-|||||||||------------------|",
        "|----------------------------|--------------------------------|||||||||-|||||||||------------------|",
        "|--------------------------------------------------------------------------------------------------|",
        "|-------|||||||||||||------|----------------------------------|||||||||-|||||||||------------------|",
        "|------||||||||||||||||---------|----------------------------||||||||||-|||||||||------------------|",
        "|-----||||||||||||||||||------|-----|||||--------------------||||||||||-|||||||||------------------|",
        "|-----||||||||||||||||||||--|-----|||||||--------O----------|||||||||||-|||||||||------------------|",
        "|-----||||||||||||||||||||||||||||||||||||-----------------||||||||||||-||||||||||||||||||||||||||||",
        "|-----||||||||||||||||||||||||||||||||||||-----------------||||||||||||-||||||||||||||||||||||||||||", // 2
        "|------||||||||||||||||||||||||||||||||||------------------||||||||||||-||||||||||||||||||||||||||||",
        "|----------||||||||||||||||||||||||||||||------------------||||||||||||-||||||||||||||||||||||||||||",
        "||--------------|||||||||||||||||||||||||-S----------------||||||||||||-||||||||||||||||||||||||||||",
        "||--------------|||||||||||||||||||||||||------------------||||||||||||-|||-|----|||||||||||||||||||",
        "||-------------||||||||||||||||||||||||||-----------S------||||||||||||-|||-------||||||||||||||||||",
        "|||-----------|||||||||||||||||||||||||||------------------||||||||||||-|||-|------|||||||||||||||||",
        "||||---------||||||||||||||||||||||||||||------------------||||||||||||-|||---------||||||||||||||||",
        "|||||---B---|||||||||||||||||||||||||||||--S---------------||||||||||||-|||-|B-------|||||||||||||||",
        "||||||-----||||||||||||||||||||||||||||||------------------||||||||||||-|||-----------||||||||||||||",
        "|||||||---|||||||||||||||||||||||||||||||------------------||||||||||||-|||-|----------|||||||||||||",
        "|||||||||||||||||||||||||||||||||||||||||-------------B----||||||||||||-|||-------------||||||||||||",
        "|||||||||||||||||||||||||||||||||||||||||------------------||||||||||||-|||-|------------|||||||||||",
        "|||||||||||||||||||||||||||||||||||||||||------------------||||||||||||-|||---------------||||||||||",
        "||||||||||||||||||||||||||||||||||||||||||---||||||||||||||||||||||||||-|||||||||----------|||||||||",
        "||||||||||||||||||||||||||||||||||||||||||---||||||||||||||||||||||||||-|||||||||-----------||||||||", // 3
        "||||||||||||||||||||||||||||||||||||||||||---S-------------||||||||||||-|||||||||------------|||||||",
        "||||||||||||||||||||||||||||||||||||||||||-----------------|||----------------|||-------------||||||",
        "||||||||||||||||||||||||||||||||||||||||||-----------------||-|--------------|-|||-------------|||||",
        "||||||||||||||||||||||||||||||||||||||||||-----------------||------------------||||-------------||||",
        "-----------------------------------------------------------||------------------|||||----------------",
        "-----------------------------------------------------------||------------------|||||||--------------",
        "||||||||||||||||||||||||||||||||||||||||||-|-|-||||||||||||||------------------|||||||||||||||||||||",
        "|||---------||||||||||||||||||||||||||||||-----||||||||||||||---------B--------|||||||||||||||||||||",
        "|---w---w-------|||||||||||||||||||||||||||||||||||||||||||||------------------|||||||||||||||||||||",
        "|---w---w-------|||||||||||||||||||||||||||||||||||||||||||||------------------|||||||||||||||||||||",
        "|---w---w-------|||||||||||||||||||||||||||||||||||||||||||||------------------|||||||||||||||||||||",
        "|---------------|||||||||||||||||||||||||||||||||||||||||||||S-----------------|||||||||||||||||||||",
        "|----w--w-------|||||||||||||||||||||||||||||||||||||||||||||------------------|||||||||||||||||||||",
        "|-----ww--------||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||",
        "|------------------||||||||||||||||||||||||||||||||||||||||||------------------|||||||||||||||||||||", // 4
        "|------------------||------------------||||||||||||||||||||||------------------|||||||||||||||||||||",
        "|------------------||------------------||||||||||||||||||||||------------------||------------------|",
        "|------------------||------------------||||||||||||||||||||||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
        "|------------------||------------------||------------------||------------------||------------------|",
    };
    // clang-format on
    Int tail_lengths[GS_ZELDA_PATHERS];
    Dir tail_dirs[GS_ZELDA_PATHERS];

    Int p_idx = 0;
    {
        Snake_Pather_Way ways[] = {
            {.dir = Dir_Right, .len = 5}, {.dir = Dir_Down, .len = 1}, {.dir = Dir_Right, .len = 1},
            {.dir = Dir_Down, .len = 5},  {.dir = Dir_Left, .len = 6}, {.dir = Dir_Up, .len = 6},
        };

        tail_lengths[p_idx] = 6;
        tail_dirs[p_idx] = Dir_Down;
        g.pathers[p_idx++] = snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
    }
    {
        Snake_Pather_Way ways[] = {{.dir = Dir_Right, .len = 4},
                                   {.dir = Dir_Down, .len = 4},
                                   {.dir = Dir_Left, .len = 4},
                                   {.dir = Dir_Up, .len = 4}};

        tail_lengths[p_idx] = 8;
        tail_dirs[p_idx] = Dir_Right;
        g.pathers[p_idx++] = snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
    }
    {
        Snake_Pather_Way ways[] = {{.dir = Dir_Right, .len = 5},
                                   {.dir = Dir_Down, .len = 4},
                                   {.dir = Dir_Left, .len = 5},
                                   {.dir = Dir_Up, .len = 4}};

        tail_lengths[p_idx] = 8;
        tail_dirs[p_idx] = Dir_Right;
        g.pathers[p_idx++] = snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
    }
    {
        Snake_Pather_Way ways[] = {{.dir = Dir_Right, .len = 4},
                                   {.dir = Dir_Down, .len = 3},
                                   {.dir = Dir_Left, .len = 4},
                                   {.dir = Dir_Up, .len = 3}};

        tail_lengths[p_idx] = 8;
        tail_dirs[p_idx] = Dir_Right;
        g.pathers[p_idx++] = snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
    }
    {
        Snake_Pather_Way ways[] = {{.dir = Dir_Right, .len = 13},
                                   {.dir = Dir_Down, .len = 5},
                                   {.dir = Dir_Left, .len = 13},
                                   {.dir = Dir_Up, .len = 5}};

        tail_lengths[p_idx] = 14;
        tail_dirs[p_idx] = Dir_Right;
        g.pathers[p_idx++] = snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
    }
    {
        Snake_Pather_Way ways[] = {{.dir = Dir_Down, .len = 1},
                                   {.dir = Dir_Right, .len = 17},
                                   {.dir = Dir_Up, .len = 1},
                                   {.dir = Dir_Left, .len = 17}};

        tail_lengths[p_idx] = 30;
        tail_dirs[p_idx] = Dir_Right;
        g.pathers[p_idx++] = snake_pather_init_except_position(ways, sizeof(ways) / sizeof(Snake_Pather_Way));
    }

    {
        Int b_idx = 0;
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
        g.boxes[b_idx++].w_h = (Pos){.x = 2, .y = 2};
        g.boxes[b_idx++].w_h = (Pos){.x = 2, .y = 2};
        g.boxes[b_idx++].w_h = (Pos){.x = 2, .y = 2};
        g.boxes[b_idx++].w_h = (Pos){.x = 1, .y = 1};
    }

    maze0_init_from_string(map, GS_ZELDA_WIDTH, GS_ZELDA_HEIGHT, NULL, (Maze0_Cell *)g.maze, g.pathers, tail_lengths,
                           tail_dirs, g.boxes, &g.w);

    *new_g = g;
}
// normal snake
Level_Return game_state_frame_Zelda(GS_Zelda *g)
{
    World_State0 *w = &g->w;
    // logic

    if (g->player_inv_timer > 0.f)
    {
        g->player_inv_timer -= GetFrameTime();
    }

    if (g->room_change_timer <= 0.f)
    {
        player_set_direction_from_input(&g->player);

        if (time_move_logic_general(&g->time_for_move, 0.105))
        {
            if (maze0_player_can_move((Maze0_Cell *)g->maze, GS_ZELDA_WIDTH, &g->player, w))
            {
                if (!boxes_collision_logic_maze0(
                        g->boxes, GS_ZELDA_MAX_BOXES,
                        move_inside_grid(player_nth_position(&g->player, 0), g->player.next_direction, w),
                        g->player.next_direction, (Pos){1, 1}, -1, (Maze0_Cell *)g->maze, GS_ZELDA_WIDTH, w))
                {
                    if (player_move(&g->player, w))
                    {
                        if (g->player_inv_timer <= 0.f)
                        {
                            g->player.length--;
                            g->player_inv_timer = GS_ZELDA_PLAYER_INV_TIME;
                        }
                    }
                }
            }

            // Calculate possible new room we shall transition to
            {
                const Pos p_pos = player_nth_position(&g->player, 0);
                const Int room_x = p_pos.x / GS_ZELDA_ROOM_WIDTH;
                const Int room_y = p_pos.y / GS_ZELDA_ROOM_HEIGHT;
                if (room_x != g->room_x || room_y != g->room_y)
                {
                    if ((-1) != g->room_x)
                    {
                        // not the first frame
                        g->room_change_timer = GS_ZELDA_ROOM_CHANGE_TIME;
                        g->prev_room_x = g->room_x;
                        g->prev_room_y = g->room_y;
                    }
                    g->room_x = room_x;
                    g->room_y = room_y;
                }
            }

            for (Int i = 0; i < GS_ZELDA_PATHERS; ++i)
            {
                snake_pather_move(&g->pathers[i], w);
            }
            if (g->player_inv_timer <= 0.f)
            {
                for (Int i = 0; i < GS_ZELDA_PATHERS; ++i)
                {
                    if (snake_pather_player_intersection(&g->pathers[i], &g->player))
                    {
                        g->player_inv_timer = GS_ZELDA_PLAYER_INV_TIME;
                        g->player.length--;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // cutscene from one room to another
        g->room_change_timer -= GetFrameTime();
    }

    Int score = GS_ZELDA_MAX_POINTS;

    for (Int i = 0; i < g->player.length; ++i) // Player collisions with buttons
    {
        Pos pos = player_nth_position(&g->player, i);
        if (g->maze[pos.y][pos.x] == Maze0_Cell_Button)
        {
            score--;
        }
    }

    for (Int i = 0; i < GS_ZELDA_MAX_BOXES; ++i)
    { // Boxes collisions with buttons
        Box *box = &g->boxes[i];
        for (Int y = box->p.y; y < box->p.y + box->w_h.y; ++y)
        {
            for (Int x = box->p.x; x < box->p.x + box->w_h.x; ++x)
            {
                if (g->maze[y][x] == Maze0_Cell_Button)
                {
                    score--;
                }
            }
        }
    }

    if (g->player.length <= 0)
        return Level_Return_Reset_Level;

    if (score <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    {
        float cam_x, cam_y;
        cam_x = w->block_pixel_len * GS_ZELDA_ROOM_WIDTH *
                (g->room_x + (g->prev_room_x - g->room_x) * (g->room_change_timer / GS_ZELDA_ROOM_CHANGE_TIME));
        cam_y = w->block_pixel_len * GS_ZELDA_ROOM_HEIGHT *
                (g->room_y + (g->prev_room_y - g->room_y) * (g->room_change_timer / GS_ZELDA_ROOM_CHANGE_TIME));

        // printf("cam_x: %f, max_x: %f\n", cam_x, max_x);
        Camera2D s = (Camera2D){
            .offset = {.x = 0.f, .y = 0.f}, .target = {.x = cam_x, .y = cam_y}, .rotation = 0.f, .zoom = 1.f};
        BeginMode2D(s);
    }
    {
        Color col = (GS_ZELDA_PLAYER_INV_TIME - g->player_inv_timer < 0.2f) ? BLACK : RAYWHITE;
        ClearBackground(col);
    }
    maze0_draw((Maze0_Cell *)g->maze, GS_ZELDA_WIDTH, GS_ZELDA_HEIGHT, w);
    for (Int i = 0; i < GS_ZELDA_PATHERS; ++i)
    {
        snake_pather_draw(&g->pathers[i], w);
    }
    draw_food_left_in_2D_space_general(score, w->width * w->block_pixel_len, w->height * w->block_pixel_len, 225, -175);

    if (g->player_inv_timer > 0.f)
    {
        player_draw_flashing(&g->player, w);
    }
    else
    {
        player_draw_extra(&g->player, w);
    }
    for (Int i = 0; i < GS_ZELDA_MAX_BOXES; ++i)
    {
        box_draw(&g->boxes[i], w);
    }

    DrawText("\" Avoid pusing boxes into unrecoverable positions...\"", 1000, 650, 20, PURPLE);

    EndMode2D();
    draw_fps();
    EndDrawing();

    return Level_Return_Continue;
}
