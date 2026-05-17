#include "nlp.h"
#include <stdio.h>

bool nlp_init(void) {
    // Stub: no model available by default
    fprintf(stderr, "NLP: stub initialized (no model available)\n");
    return false;
}

void nlp_shutdown(void) {
    // nothing to do
}

bool nlp_process_text(const char *input, char *output, int output_size) {
    (void)input;(void)output;(void)output_size;
    // Always indicate unavailable in stub
    return false;
}
