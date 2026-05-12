#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gap_buffer.h"
#include "undo_redo.h"
#include "formatter.h"
#include "exporter.h"
#include "encoding.h"
#include "config.h"
#include "search_replace.h"

int main(void) {
    enable_utf8_console();

    GapBuffer *buffer = gap_buffer_create(64);
    if (!buffer) {
        fprintf(stderr, "Erreur : impossible de créer le buffer\n");
        return 1;
    }

    UndoRedo *history = undo_redo_create();
    if (!history) {
        fprintf(stderr, "Erreur : impossible de créer l'historique\n");
        gap_buffer_destroy(buffer);
        return 1;
    }

    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, "Intelli", strlen("Intelli"));
    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, " ", 1);
    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, "Editor", strlen("Editor"));
    undo_redo_save(history, buffer);

    gap_buffer_move_cursor(buffer, 7);
    undo_redo_save(history, buffer);

    gap_buffer_insert(buffer, " C", strlen(" C"));

    char output[128];
    gap_buffer_to_string(buffer, output, sizeof(output));
    console_printf("Contenu : %s\n", output);
    console_printf("Longueur : %zu, Curseur : %zu\n",
           gap_buffer_length(buffer), gap_buffer_cursor(buffer));

    const char *source = "Français — UTF-8 test ☺";
    wchar_t *wide = utf8_to_utf16_alloc(source);
    if (wide) {
        char *roundtrip = utf16_to_utf8_alloc(wide);
        if (roundtrip) {
            console_printf("UTF-8->UTF-16->UTF-8 : %s\n", roundtrip);
            free(roundtrip);
        } else {
            fprintf(stderr, "Erreur : conversion UTF-16 vers UTF-8 échouée\n");
        }
        free(wide);
    } else {
        fprintf(stderr, "Erreur : conversion UTF-8 vers UTF-16 échouée\n");
    }

    Formatter *formatter = formatter_create();
    if (formatter) {
        formatter_set_style(formatter, 0, 6, STYLE_BOLD);       
        formatter_set_style(formatter, 6, 1, STYLE_UNDERLINE);  
        formatter_set_style(formatter, 7, 6, STYLE_ITALIC);     
        formatter_set_style(formatter, 0, 13, STYLE_HEADING_1); 

        char *styled = formatter_render_markup(formatter, output);
        if (styled) {
            console_printf("Rendu style : %s\n", styled);
            if (exporter_save_text("document.txt", output)) {
                console_printf("Export .txt réussi\n");
            }
            if (exporter_save_rtf("document.rtf", output, formatter_get_styles(formatter), formatter_style_count(formatter))) {
                console_printf("Export .rtf réussi\n");
            }
            if (exporter_save_ie("document.ie", output, formatter_get_styles(formatter), formatter_style_count(formatter))) {
                console_printf("Export .ie réussi\n");
            }
            free(styled);
        }
        formatter_destroy(formatter);
    } else {
        fprintf(stderr, "Erreur : impossible de créer le formatter\n");
    }

    Config *config = config_create();
    if (config) {
        config_set(config, "General", "language", "fr_FR");
        config_set(config, "General", "theme", "light");
        config_set(config, "Editor", "font_name", "Consolas");
        config_set(config, "Editor", "font_size", "12");
        if (config_save(config, "config.ini")) {
            console_printf("Configuration sauvegardée dans config.ini\n");
        }

        config_destroy(config);
    } else {
        fprintf(stderr, "Erreur : impossible de créer la configuration\n");
    }

    if (undo_redo_can_undo(history)) {
        undo_redo_undo(history, buffer);
        gap_buffer_to_string(buffer, output, sizeof(output));
        console_printf("Après undo : %s\n", output);
    }

    if (undo_redo_can_redo(history)) {
        undo_redo_redo(history, buffer);
        gap_buffer_to_string(buffer, output, sizeof(output));
        console_printf("Après redo : %s\n", output);
    }

    // --- Tests Recherche et Remplacement ---
    console_printf("\n--- Test Recherche ---\n");
    gap_buffer_set_text(buffer, "Le petit chat boit du lait. Un autre chat arrive.");
    
    size_t pos1 = search_find_next(buffer, "chat", 0);
    console_printf("Premier 'chat' trouve a : %zu\n", pos1);

    size_t pos2 = search_find_next(buffer, "chat", pos1 + 1);
    console_printf("Deuxieme 'chat' trouve a : %zu\n", pos2);

    size_t pos3 = search_find_prev(buffer, "chat", pos2 - 1);
    console_printf("Précédent 'chat' (depuis %zu) trouvé a : %zu\n", pos2 - 1, pos3);

    console_printf("\n--- Test Remplacement ---\n");
    size_t replaced = search_replace_all(buffer, "chat", "chien");
    gap_buffer_to_string(buffer, output, sizeof(output));
    console_printf("Remplacements effectues : %zu\n", replaced);
    console_printf("Resultat : %s\n", output);

    undo_redo_destroy(history);
    gap_buffer_destroy(buffer);
    return 0;
}