#include "game_state_StaticPlatformer.h"
#include "player_related.h"
#include "very_general.h"
#include <stdlib.h>

void metagame_set_level_StaticPlatformer(Meta_Game *mg)
{
    mg->frame_code = (Meta_Game_Frame_Code)game_state_frame_StaticPlatformer;
    mg->init_code = (Meta_Game_Init_Code)game_state_init_StaticPlatformer;
    mg->size = (sizeof(Game_State_StaticPlatformer));
}

void game_state_init_StaticPlatformer(Game_State_StaticPlatformer *new_g)
{
    Game_State_StaticPlatformer g;
    g.w = world_state0_init(SPLATFORMER_MAP_WIDTH);
    // HEIGHT 22, WIDTH 30
    g.player = (Player){.idx_pos = 1,
                        .length = 2,
                        .next_direction = Dir_Right,
                        .current_direction = Dir_Nothing,
                        .positions = {{.x = 1, .y = 20}, {.x = 2, .y = 20}}};
    g.time_for_move = 1.0;
    g.turn = false;

    const char *const map = "-----------------F-----------x"
                            "----------------FF-----------x"
                            "---------------F-F-----------x"
                            "--------------F--F-----------x"
                            "-------------F---F-----------x"
                            "-----------------x-----------x"
                            "-----------------------------x"
                            "-----------------------------x"
                            "F----------------------------x"
                            "------------------x----------x"
                            "-----------------xx----------x"
                            "--------------xxxxxxx--------x"
                            "--------x----x---------------x"
                            "---------------------------F-x"
                            "-----------------------------x"
                            "------xx--------------------xx"
                            "F----xxxxx-------------------x"
                            "-----xxxxxxx---------------x-x"
                            "-------------------------F---x"
                            "-------------------x--------xx"
                            "------------xxxxxxxxxxxxxxxxxx"
                            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    Int food_idx = 0;
    for (Int i = 0; i < SPLATFORMER_MAP_HEIGHT; ++i)
    {
        for (Int j = 0; j < SPLATFORMER_MAP_WIDTH; ++j)
        {
            char c = map[j + i * SPLATFORMER_MAP_WIDTH];

            SPlatformer_Block block = SPlatformer_Block_Air;

            switch (c)
            {
            case '-': {
            }
            break;
            case 'x': {
                block = SPlatformer_Block_Solid;
            }
            break;
            case 'F': {
                g.foods[food_idx++] = (Food){.pos = {.x = j, .y = i}};
            }
            break;
            }
            g.map[i][j] = block;
        }
    }

    *new_g = g;
}

void map_draw(const SPlatformer_Block map[], const Int map_width, const Int map_height, const World_State0 *w)
{
    for (Int i = 0; i < map_height; ++i)
    {
        for (Int j = 0; j < map_width; ++j)
        {
            SPlatformer_Block block = map[i * map_width + j];
            Color col;
            switch (block)
            {

            case SPlatformer_Block_Air: {
                continue;
            }
            break;
            case SPlatformer_Block_Solid: {
                col = (Color){0, 0, 0, 200};
            }
            break;
            }
            draw_block_at((Pos){j, i}, col, w);
        }
    }
}

Level_Return game_state_frame_StaticPlatformer(Game_State_StaticPlatformer *g)
{
    World_State0 *w = &g->w;
    // logic
    player_set_direction_from_input(&g->player);

    if (time_move_logic_general(&g->time_for_move, 0.175))
    {
        g->turn = !g->turn;

        Dir saved_dir = g->player.next_direction;

        if (g->turn)
        {
            g->player.next_direction = Dir_Down;
        }
        else
        {
            // Make sure you can only go up if all your parts are bound to something beneath them
            if (g->player.next_direction == Dir_Up)
            {
                for (Int i = 0; i + 1 < g->player.length; ++i)
                {
                    Pos pos = player_nth_position(&g->player, i);
                    Pos under_pos = move_inside_grid(pos, Dir_Down, w);
                    if (g->map[under_pos.y][under_pos.x] == SPlatformer_Block_Solid)
                    // !player_position_in_player(&g->player, under_pos))
                    {
                        // fall down one step
                        goto GOTO_ALLOWED;
                    }
                }
                g->player.next_direction = Dir_Nothing;
            GOTO_ALLOWED:;
            }
        }
        Pos head = player_nth_position(&g->player, 0);
        Pos next_pos = move_inside_grid(head, g->player.next_direction, w);
        bool next_is_solid = g->map[next_pos.y][next_pos.x] == SPlatformer_Block_Solid;
        bool next_is_snake = player_position_in_player(&g->player, next_pos);
        if (!next_is_solid && !next_is_snake)
        {
            player_move(&g->player, w);
        }

        if (g->turn)
        {
            g->player.next_direction = saved_dir;
        }

        // g->player.positions[g->player.idx_pos].y++;
        for (Int i = 0; i < SPLATFORMER_MAX_FOODS; ++i)
        {
            food_player_collision_logic_food_disappear(&g->player, &g->foods[i]);
        }
    }

    Int food_left_to_win = SPLATFORMER_MAX_FOODS + 2 - g->player.length;
    if (food_left_to_win <= 0)
        return Level_Return_Next_Level;

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(food_left_to_win);
    map_draw((SPlatformer_Block *)g->map, SPLATFORMER_MAP_WIDTH, SPLATFORMER_MAP_HEIGHT, w);

    player_draw_extra(&g->player, w);
    for (Int i = 0; i < SPLATFORMER_MAX_FOODS; ++i)
        food_draw(&g->foods[i], w);

    draw_fps();
    {
        const char *myText = "press R to reset :)";
        DrawText(myText, 150, 10, 20, PINK);
    }
    EndDrawing();
    return Level_Return_Continue;
}
