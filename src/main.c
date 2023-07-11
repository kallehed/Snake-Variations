#include "game.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "level_declarations.h"
#include "very_general.h"

// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#ifdef TEST_ALL_LEVELS
static void test_all_levels(void)
{
    SetTargetFPS(60); // DONT CHANGE, otherwise less will be seen
    for (int frame = 0;; frame += 2)
    {
        Level l = level_init(frame);

        if (NULL == l.frame_code)
        {
            break;
        }

        l.init_code(l._data);

        for (int runs = 0; runs < 240; ++runs)
        {
            meta_game_run_level_correctly(&l);
        }
        free(l._data);
        l._data = NULL;
    }
}
#endif

static void game_loop(void)
{
	Game *g = malloc(sizeof(Game));
    *g = game_init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg((void (*)(void *))game_run_frame, g, 0, 1);
#else

    SetTargetFPS(GAME_FPS); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        game_run_frame(g);
    }
    free(g->ld.l._data);
    g->ld.l._data = NULL;
#endif

	free(g);
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Variations");

#ifdef TEST_ALL_LEVELS
    test_all_levels();
#endif
    game_loop();

    CloseWindow();

    return 0;
}
