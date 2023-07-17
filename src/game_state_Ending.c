
#include "game_state_Ending.h"
#include "level.h"
#include "level_declarations.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

GS_Ending gs_init_Ending(Int score, Int deaths, Int evilness)
{
    GS_Ending g;
    g.w = world_state0_init(28);
    g.time_for_move = 1.0;

    for (Int i = 0; i < GS_ENDING_MAX_SEEKERS; ++i)
    {
        g.seeker_time_for_moves[i] = 1.0;
        g.seeker_wait_times[i] = 0.13273 + GetRandomValue(0, 100) / 100.0;
        g.seekers[i] = (Seeker_Snake){.length = GetRandomValue(6, 10), .direction = Dir_Left};
        set_positions_as_line_from_without_wrapping(g.seekers[i].positions, g.seekers[i].length, (Pos){26, 2 + i * 3},
                                                    Dir_Right);
    }
    g.start_time = GetTime();

    g.score = score;
    g.deaths = deaths;
    g.evilness = evilness;
	g.offset_time = 0.0;

    return g;
}

Level_Return gs_frame_Ending(GS_Ending *g)
{
    World_State0 *w = &g->w;
    // logic

    // For seeker
    for (Int i = 0; i < GS_ENDING_MAX_SEEKERS; ++i)
    {
        if (time_move_logic_general(&g->seeker_time_for_moves[i], g->seeker_wait_times[i]))
        {
            g->seeker_wait_times[i] = 0.05 + fabs(0.1 * sin(0.5 * GetTime()));

            if (GetRandomValue(1, 5) == 1)
            {
                g->seekers[i].direction = GetRandomValue(0, 3);
            }

            seeker_move(&g->seekers[i], w);
        }
    }

    // text stuff
    const char *end_text = "For the rest of time.\n\n\n\n\n"
                           "Wishes you long tails, and golden apples,\n\n\n\n\n"
                           "Inspired, until eternity,\n\n\n\n\n"
                           "Snakekind, forever grateful,\n\n\n\n\n"
                           "You set the world free.\n\n\n\n\n"
                           "Of the esoteric plains of will,\n\n\n\n\n"
                           "Instead, as the benevolent mind,\n\n\n\n\n"
                           "Such things are completely beneath you.\n\n\n\n\n"
                           "No more shall you be threatened,\n\n\n\n\n"
                           "For you have now ascended all others.\n\n\n\n\n"
                           "The source of all light and divinity.\n\n\n\n\n"
                           "Till the end of time you shall see,\n\n\n\n\n"
                           "Undying light melts at your fingertips.\n\n\n\n\n"
                           "Celebration forever shines towards you.\n\n\n\n\n"
                           "It's grasp has set you free.\n\n\n\n\n"
                           "The mighty world has fallen before you.";

    Int text_y = (Int)(50.0 * (GetTime() + g->offset_time - g->start_time - 95.0));

	if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_DOWN)) {
		g->offset_time += 1.5;
	}

    // if (DEV)
    //     text_y += WINDOW_HEIGHT * 8 + 200;
    // if (DEV && text_y > 0)
    // {
    //     text_y *= 2;
    // }
    bool print_second = false;
    bool print_third = false;

    char buffer0[200]; // For score, deaths and evil

    char buffer1[100]; // For grade

    const Int text_y_stop = WINDOW_HEIGHT;
    if (text_y > text_y_stop)
    {
        print_second = true;
        text_y -= text_y_stop;
        text_y /= 10;
        text_y -= 5;
        if (text_y < 0)
            text_y = 0;

        snprintf(buffer0, sizeof(buffer0) - 1,
                 "Total score: %d       \nTotal deaths: %d       \nTotal evilness: %d    ;)  \n", g->score, g->deaths,
                 g->evilness);
        buffer0[sizeof(buffer0) - 1] = '\0';

        Int buffer0_len = TextLength(buffer0);

        if (text_y < buffer0_len)
        {
            buffer0[text_y] = '\0';
        }
        else
        {
            print_third = true;
            text_y -= buffer0_len + 5;
            if (text_y < 0)
                text_y = 0;
            char grade = '0';

            grade = round((double)'F' - ((double)'F' - (double)'A') * ((double)(g->score + 100.0 * sin(g->evilness)) / (TOTAL_LEVELS * 70.0)));

            if (grade < 'A')
                grade = 'A';
            if (grade > 'F')
                grade = 'F';

            char grade_str[2] = {grade, '\0'};
            snprintf(buffer1, sizeof(buffer1) - 1, "GRADE: %s", grade_str);

            Int buffer1_len = TextLength(buffer1);
            if (text_y < buffer1_len)
            {
                buffer1[text_y] = '\0';
            }
        }
    }

    // drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_food_left(0);

    {
        const Int colors = 6;
        const Color colors_for_head[] = {SKYBLUE, BLACK, PINK, GREEN, RED, YELLOW};
        const Color colors_for_body[] = {BLUE, WHITE, VIOLET, LIME, MAROON, GOLD};
        for (Int i = 0; i < GS_ENDING_MAX_SEEKERS; ++i)
        {
            Color head = colors_for_head[i % colors];
            Color body = colors_for_body[i % colors];
            head.a = 120;
            body.a = 120;
            seeker_draw_general(&g->seekers[i], head, body, w);
        }
    }

    if (print_second)
    {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){255, 255, 255, 140});
    }

    if (!print_second)
    {
        DrawText(end_text, 6, text_y, 40, BLACK);
    }
    else
    {
        DrawText(buffer0, 50, 50, 50, BLACK);
        if (print_third)
        {
            DrawText(buffer1, 30, 400, 160, BLACK);
        }
    }

    draw_fps();
    EndDrawing();
    return 0;
}
