#ifndef NLP_ENGINE_H
#define NLP_ENGINE_H

#include "tokenizer.h"
#include "hunspell_wrap.h"

typedef struct {
    size_t token_index;
    int    is_correct;
    char **suggestions;
    int    sugg_count;
} NlpWordResult;

typedef struct {
    TokenList      tokens;
    NlpWordResult *word_results;
    size_t         word_count;
} NlpResult;

int       nlp_engine_init(const char *aff_path, const char *dic_path);
void      nlp_engine_shutdown(void);
NlpResult nlp_analyze(const char *text, size_t length);
void      nlp_result_free(NlpResult *result);

#endif
