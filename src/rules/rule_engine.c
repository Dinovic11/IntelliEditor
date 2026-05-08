#include "rule_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ============================================================
 * Utilitaire : initialisation d'un RuleResult
 * ============================================================
 */
static void init_result(RuleResult *res, const Rule *rule) {
    res->rule = rule;
    res->passed = 1;
    res->message[0] = '\0';
}

/* ============================================================
 * Vérifie si une section existe
 * ============================================================
 */
int rule_engine_check_section_exists(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
) {
    init_result(out, rule);

    const char *target = rule->target_section;
    int found = 0;

    for (size_t i = 0; i < context->section_count; ++i) {
        if (strcmp(context->sections[i], target) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        out->passed = 0;
        snprintf(out->message, sizeof(out->message),
                 "Section manquante : %s", target);
    }

    return out->passed;
}

/* ============================================================
 * Vérifie l'ordre de deux sections
 * parameter = "Introduction>Conclusion"
 * ============================================================
 */
int rule_engine_check_section_order(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
) {
    init_result(out, rule);

    if (!rule->parameter) return 1;

    char first[64], second[64];
    sscanf(rule->parameter, "%63[^>]>%63s", first, second);

    int pos1 = -1, pos2 = -1;

    for (size_t i = 0; i < context->section_count; ++i) {
        if (strcmp(context->sections[i], first) == 0)
            pos1 = (int)i;
        if (strcmp(context->sections[i], second) == 0)
            pos2 = (int)i;
    }

    if (pos1 == -1 || pos2 == -1 || pos1 >= pos2) {
        out->passed = 0;
        snprintf(out->message, sizeof(out->message),
                 "Ordre incorrect entre %s et %s", first, second);
    }

    return out->passed;
}

/* ============================================================
 * Vérifie le nombre de mots
 * ============================================================
 */
int rule_engine_check_word_count(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
) {
    init_result(out, rule);

    if (!rule->parameter) return 1;

    int limit = atoi(rule->parameter);

    if (rule->check_type == CHECK_WORD_COUNT_MIN &&
        context->total_word_count < (size_t)limit) {

        out->passed = 0;
        snprintf(out->message, sizeof(out->message),
                 "Nombre de mots insuffisant (%zu < %d)",
                 context->total_word_count, limit);
    }

    if (rule->check_type == CHECK_WORD_COUNT_MAX &&
        context->total_word_count > (size_t)limit) {

        out->passed = 0;
        snprintf(out->message, sizeof(out->message),
                 "Nombre de mots excessif (%zu > %d)",
                 context->total_word_count, limit);
    }

    return out->passed;
}

/* ============================================================
 * Vérifie une expression régulière
 * ============================================================
 */
int rule_engine_check_regex(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
) {
    init_result(out, rule);

    /* Regex non supporté sur cette plateforme */
    out->passed = 1;
    snprintf(out->message, sizeof(out->message),
             "Vérification regex non supportée sur cette plateforme");

    return out->passed;
}

/* ============================================================
 * Vérifie le format des titres (placeholder simple)
 * ============================================================
 */
int rule_engine_check_heading_format(
    const Rule *rule,
    const DocumentContext *context,
    RuleResult *out
) {
    init_result(out, rule);

    /* Exemple simple : titres en MAJUSCULE */
    for (size_t i = 0; i < context->section_count; ++i) {
        const char *s = context->sections[i];
        for (; *s; ++s) {
            if (*s >= 'a' && *s <= 'z') {
                out->passed = 0;
                snprintf(out->message, sizeof(out->message),
                         "Titre non conforme : %s",
                         context->sections[i]);
                return 0;
            }
        }
    }

    return 1;
}

/* ============================================================
 * Moteur principal
 * ============================================================
 */
RuleReport* rule_engine_run(
    const RuleSet *ruleset,
    const DocumentContext *context
) {
    if (!ruleset || !context) return NULL;

    RuleReport *report = calloc(1, sizeof(RuleReport));
    if (!report) return NULL;

    report->count = ruleset->count;
    report->results = calloc(report->count, sizeof(RuleResult));

    for (size_t i = 0; i < ruleset->count; ++i) {
        const Rule *rule = &ruleset->rules[i];
        RuleResult *res = &report->results[i];

        int passed = 1;

        switch (rule->check_type) {
            case CHECK_SECTION_EXISTS:
                passed = rule_engine_check_section_exists(rule, context, res);
                break;
            case CHECK_SECTION_ORDER:
                passed = rule_engine_check_section_order(rule, context, res);
                break;
            case CHECK_WORD_COUNT_MIN:
            case CHECK_WORD_COUNT_MAX:
                passed = rule_engine_check_word_count(rule, context, res);
                break;
            case CHECK_REGEX_FORBIDDEN:
            case CHECK_REGEX_REQUIRED:
                passed = rule_engine_check_regex(rule, context, res);
                break;
            case CHECK_HEADING_FORMAT:
                passed = rule_engine_check_heading_format(rule, context, res);
                break;
            default:
                init_result(res, rule);
                break;
        }

        if (!passed) {
            if (rule->severity == RULE_SEVERITY_ERROR) report->errors++;
            else if (rule->severity == RULE_SEVERITY_WARNING) report->warnings++;
            else report->infos++;
        }
    }

    return report;
}

/* ============================================================
 * Libération du rapport
 * ============================================================
 */
void rule_engine_free_report(RuleReport *report) {
    if (!report) return;
    free(report->results);
    free(report);
}