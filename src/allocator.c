#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>

Allo bump_allo_init(void)
{
    const Int size = 65536;

    Bump_Allo *const bump_allo = malloc(sizeof(Bump_Allo) + size * sizeof(char));
    bump_allo->bump_pointer = bump_allo->start_of_mem;
    bump_allo->alloced = 0;
    bump_allo->max_to_alloc = size;

    const Allo allo = {
        ._alloc = bump_allo_alloc, ._reset = bump_allo_reset, ._deinit = bump_allo_deinit, ._data = bump_allo};

    return allo;
}

void *bump_allo_alloc(void *_data, Int needed)
{
    Bump_Allo *this = _data;

    void *to_return = this->bump_pointer;

    // offset bump_pointer by `needed` bytes
    const Int alignment = 8;
    Int offset = needed + alignment - (needed % alignment);
    this->bump_pointer = (void *)((char *)this->bump_pointer + offset);
    this->alloced += offset;

    printf("Bump allocator used, %d bytes, total: %d\n", offset, this->alloced);

    if (this->alloced > this->max_to_alloc)
    {
        // Bad, out of memory, OOM
        // TODO
        printf("Bump allocator out of memory!!! ! \n\r\n");
    }

    return to_return;
}

void bump_allo_reset(void *_data)
{
    Bump_Allo *this = _data;

    this->bump_pointer = (void *)this->start_of_mem;
    this->alloced = 0;
    this->max_to_alloc = this->max_to_alloc;
}
void bump_allo_deinit(void *_data)
{
    Bump_Allo *this = _data;
    this->bump_pointer = NULL;
    this->alloced = 0;
    this->max_to_alloc = 0;

    free(this);
}
