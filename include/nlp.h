#ifndef NLP_H
#define NLP_H

#include <stdbool.h>

// Simple NLP/LLM interface used by Dev C. Implementations may be synchronous
// or asynchronous; this stub provides a fallback when no model is available.

bool nlp_init(void);
void nlp_shutdown(void);

// Process text and return whether a result was produced. If no model available,
// returns false.
bool nlp_process_text(const char *input, char *output, int output_size);

#endif // NLP_H
