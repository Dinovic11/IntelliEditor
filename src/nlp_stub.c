#include "nlp.h"
#include <stdio.h>

bool nlp_init(void) {
    // Stub : aucun modèle disponible par défaut
    fprintf(stderr, "NLP : stub initialisé (modèle indisponible)\n");
    return false;
}

void nlp_shutdown(void) {
    // rien à faire
}

bool nlp_process_text(const char *input, char *output, int output_size) {
    (void)input; (void)output; (void)output_size;
    // Indique systématiquement que le modèle est indisponible dans le stub
    return false;
}
