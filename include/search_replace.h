#ifndef INTELLIEDITOR_SEARCH_REPLACE_H
#define INTELLIEDITOR_SEARCH_REPLACE_H

#include "gap_buffer.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Cherche la prochaine occurrence d'une chaîne dans le buffer.
 * @param buffer Le buffer dans lequel chercher.
 * @param query La chaîne à rechercher.
 * @param start_pos Position de départ (incluse).
 * @return La position de l'occurrence, ou (size_t)-1 si non trouvé.
 */
size_t search_find_next(const GapBuffer *buffer, const char *query, size_t start_pos);

/**
 * Cherche l'occurrence précédente.
 * @param buffer Le buffer dans lequel chercher.
 * @param query La chaîne à rechercher.
 * @param start_pos Position de départ (non incluse, on cherche vers l'arrière).
 * @return La position de l'occurrence, ou (size_t)-1 si non trouvé.
 */
size_t search_find_prev(const GapBuffer *buffer, const char *query, size_t start_pos);

/**
 * Remplace une portion de texte par une nouvelle chaîne.
 * @param buffer Le buffer à modifier.
 * @param position Position du texte à remplacer.
 * @param length Longueur du texte à supprimer.
 * @param new_text Nouveau texte à insérer.
 * @return true si le remplacement a réussi.
 */
bool search_replace(GapBuffer *buffer, size_t position, size_t length, const char *new_text);

/**
 * Remplace toutes les occurrences d'une chaîne par une autre.
 * @param buffer Le buffer à modifier.
 * @param query Texte à chercher.
 * @param new_text Texte de remplacement.
 * @return Le nombre de remplacements effectués.
 */
size_t search_replace_all(GapBuffer *buffer, const char *query, const char *new_text);

#endif
