#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

typedef enum {
    TOKEN_WORD,
    TOKEN_NUMBER,
    TOKEN_PUNCT,
    TOKEN_WHITESPACE,
    TOKEN_NEWLINE,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType   type;
    const char *text;
    size_t      len;
    size_t      offset;
    int         line;
    int         col;
} Token;

typedef struct {
    Token  *tokens;
    size_t  count;
    size_t  capacity;
} TokenList;

TokenList   tokenize(const char *text, size_t length);
void        token_list_free(TokenList *list);
const char *token_type_name(TokenType type);

#endif
