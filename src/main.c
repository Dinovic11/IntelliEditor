#include <stdio.h>
#include <string.h>
#include "gap_buffer.h"
#include "undo_redo.h"
#include "encoding.h"

int main(void) {
    GapBuffer *buffer = gap_buffer_create(64);
    if (!buffer) {
        fprintf(stderr, "Erreur : impossible de créer le buffer\n");
        return 1;
    }

    UndoRedo *history = undo_redo_create();
    if (!history) {
        fprintf(stderr, "Erreur : impossible de créer l'historique\n");
        gap_buffer_destroy(buffer);
        return 1;
    }

    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, "Intelli", strlen("Intelli"));
    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, " ", 1);
    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, "Editor", strlen("Editor"));
    undo_redo_save(history, buffer);

    gap_buffer_move_cursor(buffer, 7);
    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, " C", strlen(" C"));

    char output[128];
    gap_buffer_to_string(buffer, output, sizeof(output));
    printf("Contenu : %s\n", output);
    printf("Longueur : %zu, Curseur : %zu\n",
           gap_buffer_length(buffer), gap_buffer_cursor(buffer));

    const char *source = "Français — UTF-8 test ☺";
    wchar_t *wide = utf8_to_utf16_alloc(source);
    if (wide) {
        char *roundtrip = utf16_to_utf8_alloc(wide);
        if (roundtrip) {
            printf("UTF-8->UTF-16->UTF-8 : %s\n", roundtrip);
            free(roundtrip);
        } else {
            fprintf(stderr, "Erreur : conversion UTF-16 vers UTF-8 échouée\n");
        }
        free(wide);
    } else {
        fprintf(stderr, "Erreur : conversion UTF-8 vers UTF-16 échouée\n");
    }

    if (undo_redo_can_undo(history)) {
        undo_redo_undo(history, buffer);
        gap_buffer_to_string(buffer, output, sizeof(output));
        printf("Après undo : %s\n", output);
    }

    if (undo_redo_can_redo(history)) {
        undo_redo_redo(history, buffer);
        gap_buffer_to_string(buffer, output, sizeof(output));
        printf("Après redo : %s\n", output);
    }

    undo_redo_destroy(history);
    gap_buffer_destroy(buffer);
    return 0;
}