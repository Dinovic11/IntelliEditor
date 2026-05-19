#include "memory.h"
#include <stdlib.h>

void *memory_alloc(size_t size)
{
    return malloc(size);
}
