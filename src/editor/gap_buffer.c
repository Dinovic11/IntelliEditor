#include "gap_buffer.h"
#include <stdlib.h>

struct GapBuffer { char *data; size_t size; };

GapBuffer *gap_buffer_create(void)
{
    return malloc(sizeof(GapBuffer));
}

void gap_buffer_destroy(GapBuffer *buffer)
{
    free(buffer);
}
