#include "game_cutscenes.h"
#include <math.h>
#include <stdio.h>

// Makes cutscene, and calculates score and updates global score
Cutscene_State cutscene_init(Int points_gained, Int global_score, Int cutscene_nr)
{
    Cutscene_State c;
    c.start_time = GetTime();
    c.points_gained = points_gained;
    c.global_score = global_score;
    c.cutscene_nr = cutscene_nr;
	c.cheat_state = 0;
    return c;
}

// returns true if cutscene is done
bool cutscene_frame(Cutscene_State *cs)
{
    const double time_passed = GetTime() - cs->start_time;

	if (IsKeyPressed(KEY_M)) cs->cheat_state++;

	if (cs->cheat_state >= 3) {return true;}

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(time_passed * 200 - 1300, 0, 20, 1000, LIME);
    {
        DrawText("0", 200, -40, 800, (Color){0, 0, 0, 60});
    }

    {
        const char *const texts[] = {
            "You have come so far already!",
            "Truly great feats achieved!",
            "Wow! Beautifully played!!",
            "Boxtravagantly played right there!",
            "Wow, that level couldn't get any longer!",
            "Couldn't see you there!",
            "Boxceptionally manoeuvred right there!",
            "Delightfully inverted, or?",
            "That maze sure wasn't prepared for that!",
            "Reversion is not diversion.",
            "Mario and Luigi surely must look up to you!",
            "WARNING! The greatest player ever is here!",
            "Trippy! Surely unsurreptitiously moved!",
            "Truly flabbergasting! The horror!",
            "Woooow, such great plains!",
            "Waiting? Never heard of her",
            "Disco floor sure is getting crowded!",
            "Hmph, AAAhh! Survival to be sure!",
            "Weeeeeee, snake to the moon!",
            "You are Slink, saviour of snakekind!",
            "Where did the camera-man go? Hello?",
            "Ping pang, bing dang! Lots happening!",
            "The universe is contracting onto itself",
            "Evil escalators sure escape my every ace!",
            "Wow haha, levels sure are cool now!",
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 
        };
        const char *const my_text = texts[cs->cutscene_nr];

        const Int my_text_len = TextLength(my_text);

        Int char_at = (int32_t)(12.0 * (time_passed - 2.0));
        if (char_at < 0)
            char_at = 0;

        char buffer[200];
        // first give the buffer the personal cutscene text
        snprintf(buffer, sizeof(buffer) - 1, "%s", my_text); // spare a character for \0
        buffer[sizeof(buffer) - 1] = '\0';

        const bool draw_score = char_at >= my_text_len;

        if (!draw_score)
        {
            buffer[char_at] = '\0';
        }

        DrawText(buffer, 35, 50, 30, BLACK);

        if (draw_score)
        {
            char_at -= my_text_len + 14;
            if (char_at < 0)
                char_at = 0;

            snprintf(buffer, sizeof(buffer) - 1, "\nPoints received: %d     \nTotal Points: %d", cs->points_gained,
                     cs->global_score);
            if (char_at < (Int)sizeof(buffer))
            {
                buffer[char_at] = '\0';
            }

            DrawText(buffer, 35, 50, 30, BLACK);
        }
    }
    EndDrawing();
    if (time_passed > 12.0 || (DEV && IsKeyPressed(KEY_N)))
        return true;
    return false;
}

Death_Stats_State death_stats_init(Int deaths_in_level, Int total_deaths, Int evilness)
{
    Death_Stats_State d;
    d.start_time = GetTime();
    d.deaths_in_level = deaths_in_level;
    d.total_deaths = total_deaths;
    d.evilness = evilness;
    return d;
}

bool death_stats_frame(Death_Stats_State *ds)
{
    const double time_passed = GetTime() - ds->start_time;

    BeginDrawing();
    ClearBackground(BLACK);

    const double time_til_first_text = 2.0;
    bool exit_this_cutscene = false;
    if (time_passed > time_til_first_text)
    {
        char text[] = "DEATH";
        Int char_at = (Int)(10.0 * (time_passed - time_til_first_text));

        const Int first_text_len = sizeof(text);
        const bool at_first = char_at < first_text_len;

        if (at_first)
        {
            text[char_at] = '\0';
        }

        DrawText(text, 40, 50, 220, (Color){255, 0, 0, 255});

        if (!at_first)
        {
            char_at -= first_text_len + 20;
            if (char_at < 0)
                char_at = 0;

            char buffer[100];
            snprintf(buffer, sizeof(buffer) - 1, "Total deaths: %d", ds->total_deaths);
            buffer[sizeof(buffer) - 1] = '\0';
            const Int second_text_len = TextLength(buffer);

            const bool at_second = char_at < second_text_len;

            if (at_second)
            {
                buffer[char_at] = '\0';
            }
            DrawText(buffer, 100, 300, 40, (Color){200, 200, 0, 255});

            if (!at_second)
            {
                char_at -= second_text_len + 5;
                if (char_at < 0)
                    char_at = 0;
                snprintf(buffer, sizeof(buffer) - 1, "Total evilness: %d", ds->evilness);
                buffer[sizeof(buffer) - 1] = '\0';
                const Int third_text_len = TextLength(buffer);
                const bool at_third = char_at < third_text_len;
                if (at_third)
                {
                    buffer[char_at] = '\0';
                }
                DrawText(buffer, 150, 365, 30, VIOLET);

                if (!at_third)
                {
                    char_at -= third_text_len;

                    if (char_at > 5)
                    {
                        ClearBackground((Color){100, 0, 0, 200});
                    }
                    if (char_at > 8)
                    {
                        exit_this_cutscene = true;
                    }
                }
            }
        }
    }
    EndDrawing();

    if (exit_this_cutscene)
        return true;

    return false;
}
Surprise_State surprise_init(void)
{
    Surprise_State s;
    s.points_lost = 0;
    s.start_time = GetTime();

    return s;
}
// returns true if we should exit surprise state
bool surprise_frame(Surprise_State *surp)
{
    Color background_color = BLACK;
    Color lose_color = RED;
    const double time_passed = GetTime() - surp->start_time;

    if (time_passed > 9.5)
    {
        background_color = RED;
    }
    if (time_passed > 10.0) // go back
    {
        return true;
    }

    if (get_dir_from_input() != Dir_Nothing)
    {
        surp->points_lost += 10;
        lose_color = GOLD;
    }

    BeginDrawing();
    ClearBackground(background_color);
    DrawText("STOP!", 50, 100, 250, WHITE);
    DrawText("Don't touch anything!", 100, 350, 60, YELLOW);
    {
        char text[100];
        snprintf(text, sizeof(text), "You lose %d points!", surp->points_lost);
        DrawText(text, 150, 450, 30, lose_color);
    }

    {
        double rect_y = -300.0 + 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle(0, (int)rect_y, WINDOW_WIDTH, 100, (Color){0, 255, 0, 150});
    }
    {
        double rect_x = 1200 - 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle((int)rect_x, 0, 100, WINDOW_HEIGHT, (Color){0, 255, 0, 150});
    }
    {
        double rect_y = 1700.0 - 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle(0, (int)rect_y, WINDOW_WIDTH, 100, (Color){0, 255, 0, 150});
    }
    {
        double rect_x = -1500 + 200.0 * time_passed;
        // printf("y: %f\n", rect_y);
        DrawRectangle((int)rect_x, 0, 100, WINDOW_HEIGHT, (Color){0, 255, 0, 150});
    }

    EndDrawing();
    if (DEV && IsKeyPressed(KEY_N))
    {
        return true;
    }
    return false;
}
