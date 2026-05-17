#include "editor_api.h"
#include "gap_buffer.h"
#include "exporter.h"
#include "search_replace.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

struct EditorHandle {
    GapBuffer *buffer;
};

EditorHandle *editor_create(size_t initial_capacity) {
    EditorHandle *h = memory_malloc(sizeof(EditorHandle));
    if (!h) return NULL;
    h->buffer = gap_buffer_create(initial_capacity);
    if (!h->buffer) {
        memory_free(h);
        return NULL;
    }
    return h;
}

void editor_destroy(EditorHandle *h) {
    if (!h) return;
    gap_buffer_destroy(h->buffer);
    memory_free(h);
}

bool editor_set_text(EditorHandle *h, const char *text) {
    return h && gap_buffer_set_text(h->buffer, text);
}

size_t editor_get_length(const EditorHandle *h) {
    return h ? gap_buffer_length(h->buffer) : 0;
}

size_t editor_get_cursor(const EditorHandle *h) {
    return h ? gap_buffer_cursor(h->buffer) : 0;
}

bool editor_to_string(const EditorHandle *h, char *out, size_t out_size) {
    if (!h) return false;
    gap_buffer_to_string(h->buffer, out, out_size);
    return true;
}

bool editor_save_text(EditorHandle *h, const char *path) {
    char temp[4096];
    if (!editor_to_string(h, temp, sizeof(temp))) return false;
    return exporter_save_text(path, temp);
}

bool editor_save_rtf(EditorHandle *h, const char *path) {
    char temp[4096];
    if (!editor_to_string(h, temp, sizeof(temp))) return false;
    // formatter integration would be needed for real styles; keep simple
    return exporter_save_rtf(path, temp, NULL, 0);
}

bool editor_save_ie(EditorHandle *h, const char *path) {
    char temp[4096];
    if (!editor_to_string(h, temp, sizeof(temp))) return false;
    return exporter_save_ie(path, temp, NULL, 0);
}

size_t editor_search_find_next(EditorHandle *h, const char *needle, size_t start) {
    return search_find_next(h->buffer, needle, start);
}

size_t editor_search_replace_all(EditorHandle *h, const char *needle, const char *replacement) {
    return search_replace_all(h->buffer, needle, replacement);
}
