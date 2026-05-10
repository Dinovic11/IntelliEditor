#include "llm_interface.h"
#include <llama.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static llama_model   *g_model = NULL;
static llama_context *g_ctx   = NULL;

int llm_init(const char *model_path, int n_ctx, int n_threads) {
    llama_backend_init();

    struct llama_model_params mparams = llama_model_default_params();
    g_model = llama_load_model_from_file(model_path, mparams);
    if (!g_model) {
        fprintf(stderr, "[LLM] Erreur chargement modele : %s\n", model_path);
        return 0;
    }

    struct llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx     = (n_ctx > 0)     ? (uint32_t)n_ctx     : 2048;
    cparams.n_threads = (n_threads > 0) ? n_threads            : 4;

    g_ctx = llama_new_context_with_model(g_model, cparams);
    if (!g_ctx) {
        fprintf(stderr, "[LLM] Erreur creation contexte\n");
        llama_free_model(g_model);
        g_model = NULL;
        return 0;
    }
    return 1;
}

void llm_shutdown(void) {
    if (g_ctx)   { llama_free(g_ctx);        g_ctx   = NULL; }
    if (g_model) { llama_free_model(g_model); g_model = NULL; }
    llama_backend_free();
}

int llm_is_ready(void) { return g_ctx != NULL; }

int llm_generate(const char      *prompt,
                  int              max_tokens,
                  LlmTokenCallback callback,
                  void            *userdata,
                  char            *out_buf,
                  size_t           out_size)
{
    if (!g_ctx || !prompt) return -1;

    int n_prompt_tokens = -llama_tokenize(
        g_model, prompt, (int)strlen(prompt),
        NULL, 0, 1, 0);

    llama_token *prompt_tokens = malloc(n_prompt_tokens * sizeof(llama_token));
    if (!prompt_tokens) return -1;

    llama_tokenize(g_model, prompt, (int)strlen(prompt),
                   prompt_tokens, n_prompt_tokens, 1, 0);

    llama_kv_cache_clear(g_ctx);
    struct llama_batch batch = llama_batch_get_one(prompt_tokens, n_prompt_tokens);
    if (llama_decode(g_ctx, batch) != 0) {
        free(prompt_tokens);
        return -1;
    }

    size_t out_pos = 0;
    int    n_gen   = 0;

    struct llama_sampler *sampler = llama_sampler_chain_init(
        llama_sampler_chain_default_params());
    llama_sampler_chain_add(sampler, llama_sampler_init_greedy());

    while (n_gen < max_tokens) {
        llama_token new_tok = llama_sampler_sample(sampler, g_ctx, -1);
        llama_sampler_accept(sampler, new_tok);

        if (llama_token_is_eog(g_model, new_tok)) break;


_is_eog(g_model, new_tok)) break;

        char piece[128];
        int  piece_len = llama_token_to_piece(
            g_model, new_tok, piece, sizeof(piece), 0, 0);
        if (piece_len < 0) break;
        piece[piece_len] = '\0';

        if (callback) callback(piece, userdata);

        if (out_buf && out_pos + piece_len < out_size - 1) {
            memcpy(out_buf + out_pos, piece, piece_len);
            out_pos += piece_len;
        }

        struct llama_batch nb = llama_batch_get_one(&new_tok, 1);
        if (llama_decode(g_ctx, nb) != 0) break;

        n_gen++;
    }

    if (out_buf) out_buf[out_pos] = '\0';
    llama_sampler_free(sampler);
    free(prompt_tokens);
    return n_gen;
}
