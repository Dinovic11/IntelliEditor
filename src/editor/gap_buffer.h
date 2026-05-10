#pragma once

#include <stddef.h>

typedef struct GapBuffer GapBuffer;

GapBuffer *gap_buffer_create(void);
void gap_buffer_destroy(GapBuffer *buffer);
