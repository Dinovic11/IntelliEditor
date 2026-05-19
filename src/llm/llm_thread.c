#include "llm_thread.h"
#include "llm_interface.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_PROMPT_LEN 4096
#define RESULT_BUF_LEN 8192

typedef struct {
    char              prompt[MAX_PROMPT_LEN];
    int               max_tokens;
    LlmResultCallback callback;
    void             *userdata;
} LlmRequest;

static pthread_t       g_thread;
static pthread_mutex_t g_mutex       = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_cond        = PTHREAD_COND_INITIALIZER;
static LlmRequest      g_request;
static int             g_has_request = 0;
static int             g_running     = 0;
static int             g_busy        = 0;

static void *llm_worker(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&g_mutex);
        while (!g_has_request && g_running)
            pthread_cond_wait(&g_cond, &g_mutex);

        if (!g_running) {
            pthread_mutex_unlock(&g_mutex);
            break;
        }

        LlmRequest req = g_request;
        g_has_request  = 0;
        g_busy         = 1;
        pthread_mutex_unlock(&g_mutex);

        char result[RESULT_BUF_LEN] = {0};
        int n = llm_generate(req.prompt, req.max_tokens,
                              NULL, NULL,
                              result, RESULT_BUF_LEN);

        if (req.callback)
            req.callback(result, n >= 0 ? 1 : 0, req.userdata);

        pthread_mutex_lock(&g_mutex);
        g_busy = 0;
        pthread_mutex_unlock(&g_mutex);
    }
    return NULL;
}

int llm_thread_start(void) {
    pthread_mutex_lock(&g_mutex);
    if (g_running) { pthread_mutex_unlock(&g_mutex); return 1; }
    g_running     = 1;
    g_has_request = 0;
    g_busy        = 0;
    pthread_mutex_unlock(&g_mutex);

    if (pthread_create(&g_thread, NULL, llm_worker, NULL) != 0) {
        g_running = 0;
        return 0;
    }
    return 1;
}

void llm_thread_stop(void) {
    pthread_mutex_lock(&g_mutex);
    g_running = 0;
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
    pthread_join(g_thread, NULL);
}

int llm_thread_submit(const char       *prompt,
                       int               max_tokens,
                       LlmResultCallback callback,
                       void             *userdata)
{
    if (!prompt) return 0;
    pthread_mutex_lock(&g_mutex);
    if (!g_running || g_has_request) {
        pthread_mutex_unlock(&g_mutex);
        return 0;
    }
    strncpy(g_request.prompt, prompt, MAX_PROMPT_LEN - 1);
    g_request.prompt[MAX_PROMPT_LEN - 1] = '\0';
    g_request.max_tokens = max_tokens;
    g_request.callback   = callback;
    g_request.userdata   = userdata;
    g_has_request        = 1;
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
    return 1;
}

int llm_thread_is_busy(void) {
    pthread_mutex_lock(&g_mutex);
    int b = g_busy || g_has_request;
    pthread_mutex_unlock(&g_mutex);
    return b;
}
