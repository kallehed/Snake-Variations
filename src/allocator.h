#pragma once
#include "very_general.h"


// Allocator
typedef struct Allo {
    void *(*alloc)(void*, Int);
} Allo;
