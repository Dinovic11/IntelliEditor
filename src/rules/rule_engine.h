#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#include <stddef.h>
#include "rule.h"

/* ============================================================
 * Résultat d'une règle après évaluation
 * ============================================================
 */
typedef struct {
    const Rule *rule;        /* règle évaluée */
    int passed;              /* 1 = respectée, 0 = violée */
    char message[256];       /* message explicatif */
} RuleResult;

/* ============================================================
 * Rapport global d'analyse
 * ============================================================
 */
typedef struct {
    RuleResult *results;     /* tableau des résultats */
    size_t count;            /* nombre de résultats */
    size_t errors;
    size_t warnings;
    size_t infos;
} RuleReport;

/* ============================================================
 * Contexte du document analysé
 * (fourni par Dev A / Dev B)
 * ============================================================
 */
typedef struct {
    const char *raw_text;        /* texte brut du document */
    const char **sections;       /* noms des sections */
    size_t section_count;

    /* métriques pré-calculées */
    size_t total_word_count;
} DocumentContext;

/* ============================================================
 * API publique du moteur de règles
 * ============================================================
 */

/* Analyse un document à partir d'un RuleSet */
RuleReport* rule_engine_run(
    const RuleSet *ruleset,
    const DocumentContext *context
);

/* Libère un rapport */
void rule_engine_free_report(RuleReport *report);

/* ============================================================
 * Fonctions internes spécialisées par type de règle
 * (implémentées dans rule_engine.c)
 * ============================================================
 */
int rule_engine_check_section_exists(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
);

int rule_engine_check_section_order(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
);

int rule_engine_check_word_count(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
);

int rule_engine_check_regex(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
);

int rule_engine_check_heading_format(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
);

#endif /* RULE_ENGINE_H */