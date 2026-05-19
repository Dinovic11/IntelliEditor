extern "C" {
#include "hunspell_wrap.h"
}

#include <hunspell/hunspell.hxx>
#include <cstdlib>
#include <cstring>
#include <new>

struct HunspellHandle {
    Hunspell *hs;
};

extern "C"
HunspellHandle *hunspell_create(const char *aff_path,
                                 const char *dic_path)
{
    HunspellHandle *h = new (std::nothrow) HunspellHandle;
    if (!h) return nullptr;
    h->hs = new (std::nothrow) Hunspell(aff_path, dic_path);
    if (!h->hs) { delete h; return nullptr; }
    return h;
}


;
}

extern "C"
void hunspell_destroy(HunspellHandle *handle) {
    if (!handle) return;
    delete handle->hs;
    delete handle;
}

extern "C"
int hunspell_check(HunspellHandle *handle, const char *word) {
    if (!handle || !word) return 0;
    return handle->hs->spell(word) ? 1 : 0;
}

extern "C"
int hunspell_suggest(HunspellHandle  *handle,
                      const char      *word,
                      char          ***out_suggestions)
{
    if (!handle || !word || !out_suggestions) return 0;
    std::vector<std::string> sv = handle->hs->suggest(word);
    int n = (int)sv.size();
    if (n == 0) { *out_suggestions = nullptr; return 0; }
    char *arr = (char *)malloc(n * sizeof(char *));
    if (!arr) return 0;
    for (int i = 0; i < n; i++)
        arr[i] = strdup(sv[i].c_str());
    *out_suggestions = arr;
    return n;
}

extern "C"
void hunspell_free_suggestions(char **suggestions, int count) {
    if (!suggestions) return;
    for (int i = 0; i < count; i++) free(suggestions[i]);
    free(suggestions);
}

extern "C"
void hunspell_add_word(HunspellHandle *handle, const char *word) {
    if (!handle || !word) return;
    handle->hs->add(word);
}
