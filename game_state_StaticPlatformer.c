#include "game_state_StaticPlatformer.h"
#include "player_related.h"
#include "very_general.h"

void game_state_init_StaticPlatformer(Game_State_StaticPlatformer *new_g)
{
    Game_State_StaticPlatformer g;
    g.w = world_state0_init(SPLATFORMER_MAP_WIDTH);
    // HEIGHT 22, WIDTH 30
    g.player = (Player){.idx_pos = 1,
                        .length = 2,
                        .next_direction = Dir_Right,
                        .current_direction = Dir_Nothing,
                        .positions = {{.x = 3, .y = 3}, {.x = 2, .y = 20}}};
    g.time_for_move = 1.0;
    g.turn = false;

    const char *const map = "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "------------------------------"
                            "-------------xxxxxx-----------"
                            "--------x-x-x-----------------"
                            "------------------------------"
                            "------------------------------"
                            "------xx----------------------"
                            "F----xxxxx--------------------"
                            "-----xxxxxxx------------------"
                            "-------------------------F----"
                            "------------------------------"
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

    if (time_move_logic_general(&g->time_for_move, 0.2))
    {
        g->turn = !g->turn;

        Dir saved_dir = g->player.next_direction;

        if (g->turn)
        {
            // fall down one step
            g->player.next_direction = Dir_Down;
        }
        else
        {
        }
        Pos head = player_nth_position(&g->player, 0);
        Pos next_pos = move_inside_grid(head, g->player.next_direction, w);
        bool next_is_solid = g->map[next_pos.y][next_pos.x] == SPlatformer_Block_Solid;
        bool next_is_snake = player_position_in_player(&g->player, next_pos);
        if (!next_is_solid && !next_is_snake)
        {
            // g->player.next_direction = Dir_Down;
            player_move(&g->player, w);
            // g->player.current_direction = Dir_Up;
        }

        if (g->turn)
        {

            g->player.next_direction = saved_dir;
        }

        // g->player.positions[g->player.idx_pos].y++;
        for (Int i = 0; i < SPLATFORMER_MAX_FOODS; ++i)
        {
            if (pos_equal(player_nth_position(&g->player, 0), g->foods[i].pos))
            {
                ++g->player.length;
                g->foods[i].pos = (Pos){.x = -1, .y = -1};
            }
        }
    }

    Int food_left_to_win = (DEV ? 13 : 13) - g->player.length;
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
    EndDrawing();
    return Level_Return_Continue;
}
