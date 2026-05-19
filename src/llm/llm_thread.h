#ifndef LLM_THREAD_H
#define LLM_THREAD_H

#include "llm_interface.h"

typedef void (*LlmResultCallback)(const char *result,
                                   int         success,
                                   void       *userdata);

int  llm_thread_start(void);
void llm_thread_stop(void);
int  llm_thread_submit(const char       *prompt,
                        int               max_tokens,
                        LlmResultCallback callback,
                        void             *userdata);
int  llm_thread_is_busy(void);

#endif
