#include <stdio.h>
#include "gap_buffer.h"

int main(void) {
    GapBuffer *buffer = gap_buffer_create(64);
    if (!buffer) {
        fprintf(stderr, "Erreur : impossible de créer le buffer\n");
        return 1;
    }

    gap_buffer_insert(buffer, "Intelli", 5);
    gap_buffer_insert(buffer, " ", 1);
    gap_buffer_insert(buffer, "Editor", 13);
    gap_buffer_move_cursor(buffer, 5);
    gap_buffer_insert(buffer, " C", 2);

    char output[128];
    gap_buffer_to_string(buffer, output, sizeof(output));
    printf("Contenu : %s\n", output);
    printf("Longueur : %zu, Curseur : %zu\n",
           gap_buffer_length(buffer), gap_buffer_cursor(buffer));

    gap_buffer_destroy(buffer);
    return 0;
}