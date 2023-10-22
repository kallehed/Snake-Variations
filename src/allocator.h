#pragma once
#include "very_general.h"

typedef struct {
   // where to serve memory next
   void *bump_pointer;
   // where our buffer ends, this byte should not be written to
   Int alloced;
   Int max_to_alloc;

   // start of buffer
   char start_of_mem[];
} Bump_Allo;

Allo bump_allo_init(void);

void *bump_allo_alloc(void *data, Int needed);
void bump_allo_reset(void *data);
void bump_allo_deinit(void *data);
