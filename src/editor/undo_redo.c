#include <stdlib.h>
#include <string.h>
#include "undo_redo.h"

typedef struct {
    char *text;
    size_t cursor;
} Snapshot;

struct UndoRedo {
    Snapshot *undo_stack;
    size_t undo_count;
    size_t undo_capacity;

    Snapshot *redo_stack;
    size_t redo_count;
    size_t redo_capacity;
};

static void snapshot_free(Snapshot *snapshot) {
    if (snapshot && snapshot->text) {
        free(snapshot->text);
        snapshot->text = NULL;
    }
}

static bool snapshot_init(Snapshot *snapshot, const char *text, size_t cursor) {
    if (!snapshot || !text) {
        return false;
    }
    snapshot->cursor = cursor;
    snapshot->text = strdup(text);
    return snapshot->text != NULL;
}

static bool ensure_capacity(Snapshot **stack, size_t *capacity, size_t min_capacity) {
    if (*capacity >= min_capacity) {
        return true;
    }
    size_t new_capacity = *capacity ? *capacity * 2 : 8;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    Snapshot *new_stack = realloc(*stack, new_capacity * sizeof(Snapshot));
    if (!new_stack) {
        return false;
    }
    *stack = new_stack;
    *capacity = new_capacity;
    return true;
}

UndoRedo *undo_redo_create(void) {
    UndoRedo *state = malloc(sizeof(UndoRedo));
    if (!state) {
        return NULL;
    }
    state->undo_stack = NULL;
    state->undo_count = 0;
    state->undo_capacity = 0;
    state->redo_stack = NULL;
    state->redo_count = 0;
    state->redo_capacity = 0;
    return state;
}

void undo_redo_destroy(UndoRedo *state) {
    if (!state) {
        return;
    }
    for (size_t i = 0; i < state->undo_count; ++i) {
        snapshot_free(&state->undo_stack[i]);
    }
    for (size_t i = 0; i < state->redo_count; ++i) {
        snapshot_free(&state->redo_stack[i]);
    }
    free(state->undo_stack);
    free(state->redo_stack);
    free(state);
}

void undo_redo_clear(UndoRedo *state) {
    if (!state) {
        return;
    }
    for (size_t i = 0; i < state->undo_count; ++i) {
        snapshot_free(&state->undo_stack[i]);
    }
    for (size_t i = 0; i < state->redo_count; ++i) {
        snapshot_free(&state->redo_stack[i]);
    }
    state->undo_count = 0;
    state->redo_count = 0;
}

static bool undo_redo_push_snapshot(Snapshot **stack, size_t *count, size_t *capacity,
                                   const char *text, size_t cursor) {
    if (!ensure_capacity(stack, capacity, *count + 1)) {
        return false;
    }
    Snapshot *target = &(*stack)[*count];
    if (!snapshot_init(target, text, cursor)) {
        return false;
    }
    *count += 1;
    return true;
}

static bool undo_redo_pop_snapshot(Snapshot *stack, size_t *count, Snapshot *out) {
    if (*count == 0) {
        return false;
    }
    *count -= 1;
    *out = stack[*count];
    return true;
}

static bool undo_redo_push_undo(UndoRedo *state, const char *text, size_t cursor) {
    return undo_redo_push_snapshot(&state->undo_stack, &state->undo_count, &state->undo_capacity,
                                   text, cursor);
}

static bool undo_redo_push_redo(UndoRedo *state, const char *text, size_t cursor) {
    return undo_redo_push_snapshot(&state->redo_stack, &state->redo_count, &state->redo_capacity,
                                   text, cursor);
}

static bool undo_redo_restore_snapshot(UndoRedo *state, Snapshot snapshot, GapBuffer *buffer) {
    if (!gap_buffer_set_text(buffer, snapshot.text)) {
        snapshot_free(&snapshot);
        return false;
    }
    if (!gap_buffer_move_cursor(buffer, snapshot.cursor)) {
        snapshot_free(&snapshot);
        return false;
    }
    return true;
}

bool undo_redo_save(UndoRedo *state, const GapBuffer *buffer) {
    if (!state || !buffer) {
        return false;
    }

    size_t length = gap_buffer_length(buffer);
    char *text = malloc(length + 1);
    if (!text) {
        return false;
    }
    gap_buffer_to_string(buffer, text, length + 1);
    size_t cursor = gap_buffer_cursor(buffer);

    for (size_t i = 0; i < state->redo_count; ++i) {
        snapshot_free(&state->redo_stack[i]);
    }
    state->redo_count = 0;

    bool result = undo_redo_push_undo(state, text, cursor);
    free(text);
    return result;
}

bool undo_redo_undo(UndoRedo *state, GapBuffer *buffer) {
    if (!state || !buffer || state->undo_count == 0) {
        return false;
    }

    size_t length = gap_buffer_length(buffer);
    char *text = malloc(length + 1);
    if (!text) {
        return false;
    }
    gap_buffer_to_string(buffer, text, length + 1);
    size_t cursor = gap_buffer_cursor(buffer);
    if (!undo_redo_push_redo(state, text, cursor)) {
        free(text);
        return false;
    }
    free(text);

    Snapshot snapshot;
    if (!undo_redo_pop_snapshot(state->undo_stack, &state->undo_count, &snapshot)) {
        return false;
    }

    if (!undo_redo_restore_snapshot(state, snapshot, buffer)) {
        return false;
    }
    snapshot_free(&snapshot);
    return true;
}

bool undo_redo_redo(UndoRedo *state, GapBuffer *buffer) {
    if (!state || !buffer || state->redo_count == 0) {
        return false;
    }

    size_t length = gap_buffer_length(buffer);
    char *text = malloc(length + 1);
    if (!text) {
        return false;
    }
    gap_buffer_to_string(buffer, text, length + 1);
    size_t cursor = gap_buffer_cursor(buffer);
    if (!undo_redo_push_undo(state, text, cursor)) {
        free(text);
        return false;
    }
    free(text);

    Snapshot snapshot;
    if (!undo_redo_pop_snapshot(state->redo_stack, &state->redo_count, &snapshot)) {
        return false;
    }

    if (!undo_redo_restore_snapshot(state, snapshot, buffer)) {
        return false;
    }
    snapshot_free(&snapshot);
    return true;
}

bool undo_redo_can_undo(const UndoRedo *state) {
    return state && state->undo_count > 0;
}

bool undo_redo_can_redo(const UndoRedo *state) {
    return state && state->redo_count > 0;
}
