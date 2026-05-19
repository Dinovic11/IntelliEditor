#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 64

static int is_word_char(unsigned char c) {
    return isalpha(c) || c == '-' || c == '\'';
}

static int is_punct(unsigned char c) {
    return ispunct(c) && c != '-' && c != '\'';
}

static int list_push(TokenList *list, Token tok) {
    if (list->count >= list->capacity) {
        size_t new_cap = list->capacity * 2;
        Token *new_buf = realloc(list->tokens, new_cap * sizeof(Token));
        if (!new_buf) return 0;
        list->tokens   = new_buf;
        list->capacity = new_cap;
    }
    list->tokens[list->count++] = tok;
    return 1;
}

TokenList tokenize(const char *text, size_t length) {
    TokenList list = {0};
    if (!text) return list;
    if (length == 0) length = strlen(text);

    list.tokens = malloc(INITIAL_CAPACITY * sizeof(Token));
    if (!list.tokens) return list;
    list.capacity = INITIA
>
L_CAPACITY;

    int line = 1, col = 1;
    size_t i = 0;

    while (i < length) {
        unsigned char c = (unsigned char)text[i];
        Token tok = {0};
        tok.text   = text + i;
        tok.offset = i;
        tok.line   = line;
        tok.col    = col;

        if (c == '\n') {
            tok.type = TOKEN_NEWLINE;
            tok.len  = 1;
            line++; col = 1; i++;
        } else if (c == ' ' || c == '\t' || c == '\r') {
            tok.type = TOKEN_WHITESPACE;
            size_t start = i;
            while (i < length && (text[i]==' ' || text[i]=='\t' || text[i]=='\r'))
                { i++; col++; }
            tok.len = i - start;
        } else if (is_word_char(c)) {
            tok.type = TOKEN_WORD;
            size_t start = i;
            while (i < length && is_word_char((unsigned char)text[i]))
                { i++; col++; }
            tok.len = i - start;
        } else if (isdigit(c)) {
            tok.type = TOKEN_NUMBER;
            size_t start = i;
            while (i < length && (isdigit((unsigned char)text[i]) || text[i]=='.'))
                { i++; col++; }
            tok.len = i - start;
        } else if (is_punct(c)) {
            tok.type = TOKEN_PUNCT;
            tok.len  = 1;
            i++; col++;
        } else {
            tok.type = TOKEN_UNKNOWN;
            tok.len  = 1;
            i++; col++;
        }

        if (!list_push(&list, tok)) return list;
    }
    return list;
}

void token_list_free(TokenList *list) {
    if (!list) return;
    free(list->tokens);
    list->tokens   = NULL;
    list->count    = 0;
    list->capacity = 0;
}

const char *token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_WORD:       return "WORD";
        case TOKEN_NUMBER:     return "NUMBER";
        case TOKEN_PUNCT:      return "PUNCT";
        case TOKEN_WHITESPACE: return "WHITESPACE";
        case TOKEN_NEWLINE:    return "NEWLINE";
        default:               return "UNKNOWN";
    }
}
