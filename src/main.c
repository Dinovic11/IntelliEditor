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
#include "memory.h"
#include "nlp.h"

int main(int argc, char **argv) {
    bool standalone = false;
    bool nlp_test = false;
    const char *nlp_input = "Donne un court résumé de IntelliEditor en français.";
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--standalone") == 0 || strcmp(argv[i], "-s") == 0) {
            standalone = true;
        }
        if (strcmp(argv[i], "--nlp-test") == 0 || strcmp(argv[i], "--llm-test") == 0) {
            nlp_test = true;
        }
    }

    enable_utf8_console();
    memory_init();

    if (standalone) {
        printf("Mode: base autonome (standalone)\n");
    } else {
        printf("Mode: normal (headless run)\n");
    }

    if (nlp_test) {
        if (nlp_init()) {
            char nlp_output[512] = {0};
            if (nlp_process_text(nlp_input, nlp_output, (int)sizeof(nlp_output))) {
                printf("NLP : résultat disponible -> %s\n", nlp_output);
            } else {
                printf("NLP : le modèle est indisponible ou n'a pas retourné de résultat.\n");
            }
            nlp_shutdown();
        } else {
            printf("NLP : modèle indisponible, mode autonome activé.\n");
        }
    }

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
    printf("Contenu : %s\n", output);
    printf("Longueur : %zu, Curseur : %zu\n",
           gap_buffer_length(buffer), gap_buffer_cursor(buffer));

    const char *source = "Français — UTF-8 test ☺";
    wchar_t *wide = utf8_to_utf16_alloc(source);
    if (wide) {
        char *roundtrip = utf16_to_utf8_alloc(wide);
        if (roundtrip) {
            printf("UTF-8->UTF-16->UTF-8 : %s\n", roundtrip);
            memory_free(roundtrip);
        } else {
            fprintf(stderr, "Erreur : conversion UTF-16 vers UTF-8 échouée\n");
        }
        memory_free(wide);
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
            printf("Rendu style : %s\n", styled);
            if (exporter_save_text("document.txt", output)) {
                printf("Export .txt réussi\n");
            }
            if (exporter_save_rtf("document.rtf", output, formatter_get_styles(formatter), formatter_style_count(formatter))) {
                printf("Export .rtf réussi\n");
            }
            if (exporter_save_ie("document.ie", output, formatter_get_styles(formatter), formatter_style_count(formatter))) {
                printf("Export .ie réussi\n");
            }
            memory_free(styled);
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
            printf("Configuration sauvegardée dans config.ini\n");
        }

        config_destroy(config);
    } else {
        fprintf(stderr, "Erreur : impossible de créer la configuration\n");
    }

    if (undo_redo_can_undo(history)) {
        undo_redo_undo(history, buffer);
        gap_buffer_to_string(buffer, output, sizeof(output));
        printf("Après undo : %s\n", output);
    }

    if (undo_redo_can_redo(history)) {
        undo_redo_redo(history, buffer);
        gap_buffer_to_string(buffer, output, sizeof(output));
        printf("Après redo : %s\n", output);
    }

    printf("\n--- Test Rechercher/Remplacer ---\n");
    GapBuffer *search_buffer = gap_buffer_create(128);
    if (search_buffer) {
        gap_buffer_set_text(search_buffer, "Bonjour le monde, bonjour la vie");
        gap_buffer_to_string(search_buffer, output, sizeof(output));
        printf("Buffer initial : %s\n", output);
        
        size_t pos = search_find_next(search_buffer, "bonjour", 0);
        if (pos != (size_t)-1) {
            printf("Première occurrence de 'bonjour' à position : %zu\n", pos);
        }
        
        size_t replaced = search_replace_all(search_buffer, "bonjour", "salut");
        gap_buffer_to_string(search_buffer, output, sizeof(output));
        printf("Après remplacement (x%zu) : %s\n", replaced, output);
        
        gap_buffer_destroy(search_buffer);
    }

    undo_redo_destroy(history);
    gap_buffer_destroy(buffer);
    return 0;
}