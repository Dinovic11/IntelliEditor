#include "nlp_engine.h"
#include "tokenizer.h"
#include "hunspell_wrap.h"
#include <stdlib.h>
#include <string.h>

static HunspellHandle *g_hunspell = NULL;

int nlp_engine_init(const char *aff_path, const char *dic_path) {
    if (g_hunspell) return 1;
    g_hunspell = hunspell_create(aff_path, dic_path);
    return g_hunspell ? 1 : 0;
}

void nlp_engine_shutdown(void) {
    if (g_hunspell) {
        hunspell_destroy(g_hunspell);
        g_hunspell = NULL;
    }
}

NlpResult nlp_analyze(const char *text, size_t length) {
    NlpResult result = {0};

    result.tokens = tokenize(text, length);
    if (!result.tokens.tokens) return result;

    size_t word_cap = 0;
    for (size_t i = 0; i < result.tokens.count; i++) {
        if (result.tokens.tokens[i].type == TOKEN_WORD)
            word_cap++;
    }
    if (word_cap == 0) return result;

    result.word_results = calloc(word_cap, sizeof(NlpWordResult));
    if (!result.word_results) {
        token_list_free(&result.tokens);
        return result;
    }

    size_t wi = 0;
    for (size_t i = 0; i < result.tokens.count; i++) {
        Token *tok = &result.tokens.tokens[i];
        if (tok->type != TOKEN_WORD) continue;

        char *word = strndup(tok->text, tok->len);
        if (!word) continue;

        NlpWordResult *wr = &result.word_results[wi++];
        wr->token_index = i;

        if (g_hunspell) {
            wr->is_correct = hunspell_check(g_hunspell, word);
            if (!wr->is_correct) {
                wr->sugg_count = hunspell_suggest(
                    g_hunspell, word, &wr->suggestions);
            }
        } else {
            wr->is_correct = 1;
        }
        free(word);
    }
    result.word_count = wi;
    return result;
}

void nlp_result_free(NlpResult *result) {
    if (!result) return;
    token_list_free(&result->tokens);
    for (size_t i = 0; i < result->word_count; i++) {
        hunspell_free_suggestions(
            result->word_results[i].suggestions,
            result->word_results[i].sugg_count);
    }
    free(result->word_results);
    result->word_results = NULL;
    result->word_count   = 0;
}
