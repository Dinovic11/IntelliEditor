#include <stdio.h>
#include <string.h>
#include "editor_api.h"

int main(void) {
    EditorHandle *h = editor_create(128);
    if (!h) { printf("FAILED: editor_create returned NULL\n"); return 1; }

    const char *txt = "Bonjour test";
    if (!editor_set_text(h, txt)) { printf("FAILED: editor_set_text\n"); return 1; }

    size_t len = editor_get_length(h);
    if (len != strlen(txt)) { printf("FAILED: length (%zu) != expected (%zu)\n", len, strlen(txt)); return 1; }

    char buf[256] = {0};
    editor_to_string(h, buf, sizeof(buf));
    if (strcmp(buf, txt) != 0) { printf("FAILED: to_string mismatch '%s'\n", buf); return 1; }

    size_t found = editor_search_find_next(h, "test", 0);
    if (found == (size_t)-1) { printf("FAILED: search_find_next could not find 'test'\n"); return 1; }

    size_t replaced = editor_search_replace_all(h, "test", "essai");
    if (replaced != 1) { printf("FAILED: replace_all expected 1 got %zu\n", replaced); return 1; }

    editor_to_string(h, buf, sizeof(buf));
    if (strstr(buf, "essai") == NULL) { printf("FAILED: replacement not applied '%s'\n", buf); return 1; }

    printf("All editor_api tests passed\n");
    editor_destroy(h);
    return 0;
}
