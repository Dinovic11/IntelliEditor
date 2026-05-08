#include <stdio.h>
#include "rules/rule_parser.h"
#include "rules/rule_engine.h"

int main(void) {
    /* -----------------------------
     * Document fictif
     * ----------------------------- */
    const char *sections[] = {
    "INTRODUCTION",
    /* "METHODOLOGIE", */   // volontairement absente
    "CONCLUSION"
};

int section_count = sizeof(sections) / sizeof(sections[0]);

    const char *text =
        "Ceci est un document de test. "
        "Il contient volontairement peu de mots "
        "afin de déclencher certaines règles.";

    DocumentContext context = {
        .raw_text = text,
        .sections = sections,
        .section_count = 3,
        .total_word_count = 42
    };

    /* -----------------------------
     * Chargement des règles
     * ----------------------------- */
    RuleSet *ruleset = rule_parser_load("ruleset.json");
    if (!ruleset) {
        printf("Erreur lors du chargement des règles\n");
        return 1;
    }

    /* -----------------------------
     * Exécution du moteur
     * ----------------------------- */
    RuleReport *report = rule_engine_run(ruleset, &context);
    if (!report) {
        printf("Erreur lors de l'analyse\n");
        rule_parser_free(ruleset);
        return 1;
    }

    /* -----------------------------
     * Affichage du rapport
     * ----------------------------- */
    printf("=== RAPPORT D'ANALYSE ===\n");

    for (size_t i = 0; i < report->count; ++i) {
        RuleResult *r = &report->results[i];
        printf("[%s] %s : %s\n",
               r->passed ? "OK" : "FAIL",
               r->rule->id,
               r->passed ? "Règle respectée" : r->message);
    }

    printf("\nRésumé : %zu erreurs, %zu warnings, %zu infos\n",
           report->errors, report->warnings, report->infos);

    /* -----------------------------
     * Nettoyage
     * ----------------------------- */
    rule_engine_free_report(report);
    rule_parser_free(ruleset);

    return 0;
}