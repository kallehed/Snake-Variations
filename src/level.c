#include "level.h"
#include "level_declarations.h"
#include "music.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static Set_Level_Code get_set_level_code_from_enum(Level_Enum e)
{
    if (e >= TOTAL_LEVELS || e < 0)
    {
        printf("--------\n");
        printf("VERY BAD DEATH!!!!!!!!! AHHHHHHHH LEVEL NOT EXIST\n");
        printf("--------\n");
        return NULL;
    }

    static const Set_Level_Code LEVEL_SET_FUNCS[TOTAL_LEVELS] = {
        level_set_First,
        level_set_BlueSnakes,
        level_set_Skin,
        level_set_Boxes,
        level_set_EverGrowing,
        level_set_GigFreeFast, // custom music
        level_set_HidingBoxes,
        level_set_YouFood,
        level_set_Maze,
        level_set_GetSmall,
        level_set_StaticPlatformer,
        level_set_Seeker,
        level_set_UnSync,
        level_set_Spinny,
        level_set_OpenWorld,
        level_set_Wait,
        level_set_Suicide,
        level_set_Attack,
        level_set_Accel,
        level_set_Zelda,
        level_set_InZoom,
        level_set_ObsCourse,
        level_set_YouBlue,
        level_set_FallFood,
        level_set_OnceMaze,
        level_set_Final,
    };
    return LEVEL_SET_FUNCS[e];
}

// frees previous level that was there
void level_init(Level *l, const Level_Enum level_enum)
{
    free(l->_data);
    l->_data = NULL;

    Set_Level_Code setter = get_set_level_code_from_enum(level_enum);
    if (NULL == setter)
    {
        l->size = 0;
        l->_data = NULL;
        l->frame_code = NULL;
        l->init_code = NULL;
        return;
    }
    setter(l);

    // INIT
    printf("Mallocing size: %u\n", l->size);
    l->_data = malloc(l->size);
    if (NULL == l->_data) // Check for Out Of Memory
        puts("!!!!!!!!! OUT OF MEMORY, MALLOC RETURNED NULL!!!!!!!!");

    l->init_code(l->_data);
}

void level_data_init(Level_Data *ld, Level_Enum level_enum)
{
    ld->deaths_in_level = 0;
    ld->time_of_level_start = GetTime();
    ld->deaths_in_level = 0;
    ld->level_enum = level_enum;
    ld->death_wait_timer = 0.f;
    level_init(&ld->l, ld->level_enum);
}

// handles just resetting and stuff, returns whether level was completed or not
// DOES NOT FREE ANYTHING, just handles resets and the like
Level_Return level_run_correctly(Level *l)
{

    if (IsKeyPressed(KEY_R))
    {
        goto GOTO_RESET_LEVEL;
    }
    switch (l->frame_code(l->_data))
    {
    case Level_Return_Continue: {
        if (DEV) // hacks
        {
            if (IsKeyPressed(KEY_N))
                return Level_Return_Next_Level;
        }
        return Level_Return_Continue;
    }
    break;
    case Level_Return_Next_Level: {
        TraceLog(LOG_INFO, "%s", "Go To Next Level\n");
        return Level_Return_Next_Level;
    }
    break;
    case Level_Return_Reset_Level: {
    GOTO_RESET_LEVEL:
        l->init_code(l->_data);
        return Level_Return_Reset_Level;
    }
    break;
    }
    return Level_Return_Reset_Level; // control flow CANT go here
}

// gives the score depending on how long the player took to complete the level
Int level_data_get_score(Level_Data *ld)
{
    double time_passed = GetTime() - ld->time_of_level_start;
    double score = 100.0 / (sqrt(0.02 * time_passed + 1.0));
    Int int_score = (Int)score;
    TraceLog(LOG_INFO, "time passed: %f, score: %f, int_score: %d\n", time_passed, score, int_score);
    return int_score;
}

Music_Enum level_get_music(Level_Enum level_enum)
{

    switch (level_enum)
    {
    case Level_Final:
        return Music_Snake_Final;
	case Level_OnceMaze:
		return Music_Snake_Weird;
    case Level_YouBlue:
        return Music_Snake_Mess;
    case Level_Zelda:
        return Music_Snake_Church;
    case Level_Attack:
        return Music_Snake_Doom;
    case Level_OpenWorld:
        return Music_Snake_Scary;
    case Level_UnSync:
        return Music_Snake_Base_Headed;
    case Level_StaticPlatformer:
        return Music_Snake_Drummy;
    case Level_GigFreeFast:
        return Music_Snake_Base_Headed;
    case Level_YouFood:
        return Music_Snake_Weird;
    default: {
        return Music_Snake_Basic;
    }
    break;
    }
}
