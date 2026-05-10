#ifndef HUNSPELL_WRAP_H
#define HUNSPELL_WRAP_H

#include <stddef.h>

typedef struct HunspellHandle HunspellHandle;

HunspellHandle *hunspell_create(const char *aff_path,
                                 const char *dic_path);
void            hunspell_destroy(HunspellHandle *handle);
int             hunspell_check(HunspellHandle *handle, const char *word);
int             hunspell_suggest(HunspellHandle  *handle,
                                  const char      *word,
                                  char          ***out_suggestions);
void            hunspell_free_suggestions(char **suggestions, int count);
void            hunspell_add_word(HunspellHandle *handle, const char *word);

#endif
