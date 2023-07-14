#pragma once
#include "very_general.h"
#include <raylib.h>
#include "music.h"

// For enumerating levels
typedef enum {
    Level_First = 0,
    Level_BlueSnakes,
    Level_Skin,
    Level_Boxes,
    Level_EverGrowing,
    Level_GigFreeFast,
    Level_HidingBoxes,
    Level_YouFood,
    Level_Maze,
    Level_GetSmall,
    Level_StaticPlatformer,
    Level_Seeker,
    Level_UnSync,
    Level_Spinny,
    Level_OpenWorld,
    Level_Wait,
    Level_Suicide,
    Level_Attack,
    Level_Accel,
    Level_Zelda,
    Level_InZoom,
    Level_ObsCourse,
    Level_YouBlue,
    Level_FallFood,
    Level_OnceMaze,
    Level_Final,
	TOTAL_LEVELS
} Level_Enum;

#define LEVEL_DATA_DEATH_WAIT_TIME 0.3f
typedef struct
{
    Level l;
    Level_Enum level_enum; // what level we are at
    double time_of_level_start;
    Int deaths_in_level;
	// for showing a short cutscene after dying
	float death_wait_timer;
} Level_Data;


void level_init(Level *l, const Level_Enum level_enum);

void level_data_init(Level_Data *ld, const Level_Enum level_enum);
// handles just resetting and stuff, returns whether level was completed or not
// DOES NOT FREE ANYTHING, just handles resets and the like
Level_Return level_run_correctly(Level *l);

// gives the score depending on how long the player took to complete the level
Int level_data_get_score(Level_Data *ld);

Music_Enum level_get_music(Level_Enum level_enum);
