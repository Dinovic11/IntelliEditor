#ifndef EDITOR_API_H
#define EDITOR_API_H

#include <stdbool.h>
#include <stddef.h>

// Minimal editor API to decouple UI and NLP layers.
// Dev B (Win32 UI) should call these functions to interact with the editor core.

typedef struct EditorHandle EditorHandle;

EditorHandle *editor_create(size_t initial_capacity);
void editor_destroy(EditorHandle *h);

bool editor_set_text(EditorHandle *h, const char *text);
size_t editor_get_length(const EditorHandle *h);
size_t editor_get_cursor(const EditorHandle *h);
bool editor_to_string(const EditorHandle *h, char *out, size_t out_size);

// Persistence helpers used by UI or tooling
bool editor_save_text(EditorHandle *h, const char *path);
bool editor_save_rtf(EditorHandle *h, const char *path);
bool editor_save_ie(EditorHandle *h, const char *path);

// Simple search/replace helpers
size_t editor_search_find_next(EditorHandle *h, const char *needle, size_t start);
size_t editor_search_replace_all(EditorHandle *h, const char *needle, const char *replacement);

#endif // EDITOR_API_H
