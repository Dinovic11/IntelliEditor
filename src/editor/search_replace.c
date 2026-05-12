#include "search_replace.h"
#include <string.h>
#include <stdlib.h>

size_t search_find_next(const GapBuffer *buffer, const char *query, size_t start_pos) {
    if (!buffer || !query) return (size_t)-1;
    
    size_t query_len = strlen(query);
    if (query_len == 0) return (size_t)-1;
    
    size_t buffer_len = gap_buffer_length(buffer);
    if (start_pos + query_len > buffer_len) return (size_t)-1;

    for (size_t i = start_pos; i <= buffer_len - query_len; ++i) {
        bool match = true;
        for (size_t j = 0; j < query_len; ++j) {
            if (gap_buffer_get_at(buffer, i + j) != query[j]) {
                match = false;
                break;
            }
        }
        if (match) return i;
    }

    return (size_t)-1;
}

size_t search_find_prev(const GapBuffer *buffer, const char *query, size_t start_pos) {
    if (!buffer || !query) return (size_t)-1;
    
    size_t query_len = strlen(query);
    if (query_len == 0) return (size_t)-1;
    
    size_t buffer_len = gap_buffer_length(buffer);
    if (buffer_len < query_len) return (size_t)-1;

    // Ajuster start_pos si elle dépasse
    if (start_pos > buffer_len - query_len) {
        start_pos = buffer_len - query_len;
    }

    for (size_t i = start_pos; ; --i) {
        bool match = true;
        for (size_t j = 0; j < query_len; ++j) {
            if (gap_buffer_get_at(buffer, i + j) != query[j]) {
                match = false;
                break;
            }
        }
        if (match) return i;
        if (i == 0) break;
    }

    return (size_t)-1;
}

bool search_replace(GapBuffer *buffer, size_t position, size_t length, const char *new_text) {
    if (!buffer || !new_text) return false;
    
    // 1. Aller à la position
    if (!gap_buffer_move_cursor(buffer, position)) return false;
    
    // 2. Supprimer l'ancien texte
    // Note: gap_buffer_delete supprime ce qui est AVANT le curseur ?
    // Voyons gap_buffer.c : buffer->gap_start -= count; Oui.
    // Donc on doit d'abord avancer le curseur de 'length' pour supprimer vers l'arrière
    if (!gap_buffer_move_cursor(buffer, position + length)) return false;
    if (!gap_buffer_delete(buffer, length)) return false;
    
    // 3. Insérer le nouveau texte
    return gap_buffer_insert(buffer, new_text, strlen(new_text));
}

size_t search_replace_all(GapBuffer *buffer, const char *query, const char *new_text) {
    if (!buffer || !query || !new_text) return 0;
    
    size_t count = 0;
    size_t query_len = strlen(query);
    size_t replace_len = strlen(new_text);
    size_t pos = 0;

    while ((pos = search_find_next(buffer, query, pos)) != (size_t)-1) {
        if (search_replace(buffer, pos, query_len, new_text)) {
            count++;
            pos += replace_len; // Avancer pour éviter les boucles infinies
        } else {
            break;
        }
    }

    return count;
}
