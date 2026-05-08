#ifndef INTELLIEDITOR_UNDO_REDO_H
#define INTELLIEDITOR_UNDO_REDO_H

#include <stdbool.h>
#include "gap_buffer.h"

typedef struct UndoRedo UndoRedo;

UndoRedo *undo_redo_create(void);
void undo_redo_destroy(UndoRedo *state);

bool undo_redo_save(UndoRedo *state, const GapBuffer *buffer);
bool undo_redo_undo(UndoRedo *state, GapBuffer *buffer);
bool undo_redo_redo(UndoRedo *state, GapBuffer *buffer);

bool undo_redo_can_undo(const UndoRedo *state);
bool undo_redo_can_redo(const UndoRedo *state);

void undo_redo_clear(UndoRedo *state);

#endif
