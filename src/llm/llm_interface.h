#ifndef LLM_INTERFACE_H
#define LLM_INTERFACE_H

#include <stddef.h>

typedef void (*LlmTokenCallback)(const char *token, void *userdata);

int  llm_init(const char *model_path, int n_ctx, int n_threads);
void llm_shutdown(void);
int  llm_is_ready(void);

int llm_generate(const char      *prompt,
                  int              max_tokens,
                  LlmTokenCallback callback,
                  void            *userdata,
                  char            *out_buf,
                  size_t           out_size);

#endif
