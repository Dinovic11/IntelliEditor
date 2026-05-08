#include <stdlib.h>
#include <string.h>
#include "gap_buffer.h"

struct GapBuffer {
    char *data;
    size_t gap_start;
    size_t gap_end;
    size_t capacity;
};

static size_t gap_buffer_gap_size(const GapBuffer *buffer) {
    return buffer->gap_end - buffer->gap_start;
}

static bool gap_buffer_resize(GapBuffer *buffer, size_t min_capacity) {
    size_t current_len = gap_buffer_gap_size(buffer);
    size_t used = buffer->capacity - current_len;
    size_t new_capacity = buffer->capacity ? buffer->capacity * 2 : 64;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    char *new_data = malloc(new_capacity);
    if (!new_data) {
        return false;
    }

    size_t prefix_size = buffer->gap_start;
    size_t suffix_size = buffer->capacity - buffer->gap_end;

    memcpy(new_data, buffer->data, prefix_size);
    memcpy(new_data + new_capacity - suffix_size, buffer->data + buffer->gap_end, suffix_size);

    free(buffer->data);
    buffer->data = new_data;
    buffer->gap_end = new_capacity - suffix_size;
    buffer->capacity = new_capacity;
    return true;
}

GapBuffer *gap_buffer_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = 64;
    }

    GapBuffer *buffer = malloc(sizeof(GapBuffer));
    if (!buffer) {
        return NULL;
    }

    buffer->capacity = initial_capacity;
    buffer->data = malloc(buffer->capacity);
    if (!buffer->data) {
        free(buffer);
        return NULL;
    }

    buffer->gap_start = 0;
    buffer->gap_end = buffer->capacity;
    return buffer;
}

void gap_buffer_destroy(GapBuffer *buffer) {
    if (!buffer) {
        return;
    }
    free(buffer->data);
    free(buffer);
}

size_t gap_buffer_length(const GapBuffer *buffer) {
    return buffer->capacity - gap_buffer_gap_size(buffer);
}

size_t gap_buffer_capacity(const GapBuffer *buffer) {
    return buffer->capacity;
}

size_t gap_buffer_cursor(const GapBuffer *buffer) {
    return buffer->gap_start;
}

static void gap_buffer_move_gap(GapBuffer *buffer, size_t new_position) {
    size_t gap_size = gap_buffer_gap_size(buffer);

    if (new_position == buffer->gap_start) {
        return;
    }

    if (new_position < buffer->gap_start) {
        size_t move_size = buffer->gap_start - new_position;
        memmove(buffer->data + buffer->gap_end - move_size,
                buffer->data + new_position,
                move_size);
        buffer->gap_start = new_position;
        buffer->gap_end = buffer->gap_start + gap_size;
    } else {
        size_t move_size = new_position - buffer->gap_start;
        memmove(buffer->data + buffer->gap_start,
                buffer->data + buffer->gap_end,
                move_size);
        buffer->gap_start = new_position;
        buffer->gap_end = buffer->gap_start + gap_size;
    }
}

bool gap_buffer_move_cursor(GapBuffer *buffer, size_t position) {
    size_t length = gap_buffer_length(buffer);
    if (position > length) {
        return false;
    }
    gap_buffer_move_gap(buffer, position);
    return true;
}

bool gap_buffer_insert(GapBuffer *buffer, const char *text, size_t length) {
    if (!buffer || !text) {
        return false;
    }

    size_t gap_size = gap_buffer_gap_size(buffer);
    if (gap_size < length) {
        if (!gap_buffer_resize(buffer, buffer->capacity + length)) {
            return false;
        }
    }

    memcpy(buffer->data + buffer->gap_start, text, length);
    buffer->gap_start += length;
    return true;
}

bool gap_buffer_insert_char(GapBuffer *buffer, char c) {
    return gap_buffer_insert(buffer, &c, 1);
}

bool gap_buffer_delete(GapBuffer *buffer, size_t count) {
    if (!buffer) {
        return false;
    }

    size_t available = buffer->gap_start;
    if (count > available) {
        count = available;
    }

    buffer->gap_start -= count;
    return true;
}

bool gap_buffer_set_text(GapBuffer *buffer, const char *text) {
    if (!buffer || !text) {
        return false;
    }

    size_t length = strlen(text);
    size_t new_capacity = 64;
    while (new_capacity < length * 2 + 1) {
        new_capacity *= 2;
    }

    char *new_data = malloc(new_capacity);
    if (!new_data) {
        return false;
    }

    memcpy(new_data, text, length);
    free(buffer->data);

    buffer->data = new_data;
    buffer->capacity = new_capacity;
    buffer->gap_start = length;
    buffer->gap_end = new_capacity;
    return true;
}

size_t gap_buffer_to_string(const GapBuffer *buffer, char *dest, size_t dest_size) {
    if (!buffer || !dest || dest_size == 0) {
        return 0;
    }

    size_t prefix_size = buffer->gap_start;
    size_t suffix_size = buffer->capacity - buffer->gap_end;
    size_t total_length = prefix_size + suffix_size;

    if (dest_size <= total_length) {
        return total_length;
    }

    memcpy(dest, buffer->data, prefix_size);
    memcpy(dest + prefix_size, buffer->data + buffer->gap_end, suffix_size);
    dest[total_length] = '\0';
    return total_length;
}
