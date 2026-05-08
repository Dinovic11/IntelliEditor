#ifndef INTELLIEDITOR_GAP_BUFFER_H
#define INTELLIEDITOR_GAP_BUFFER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct GapBuffer GapBuffer;

GapBuffer *gap_buffer_create(size_t initial_capacity);
void gap_buffer_destroy(GapBuffer *buffer);

size_t gap_buffer_length(const GapBuffer *buffer);
size_t gap_buffer_capacity(const GapBuffer *buffer);
size_t gap_buffer_cursor(const GapBuffer *buffer);

bool gap_buffer_move_cursor(GapBuffer *buffer, size_t position);
bool gap_buffer_insert(GapBuffer *buffer, const char *text, size_t length);
bool gap_buffer_insert_char(GapBuffer *buffer, char c);
bool gap_buffer_delete(GapBuffer *buffer, size_t count);

bool gap_buffer_set_text(GapBuffer *buffer, const char *text);

size_t gap_buffer_to_string(const GapBuffer *buffer, char *dest, size_t dest_size);

#endif
