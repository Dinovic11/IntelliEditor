#ifndef INTELLIEDITOR_RULE_H
#define INTELLIEDITOR_RULE_H

#include <stddef.h>   // size_t

/* ============================================================
 * ENUMS
 * ============================================================ */

/* Sévérité d'une règle (impact académique) */
typedef enum {
    RULE_SEVERITY_INFO = 0,
    RULE_SEVERITY_WARNING,
    RULE_SEVERITY_ERROR
} RuleSeverity;

/* Type de vérification à effectuer */
typedef enum {
    CHECK_SECTION_EXISTS = 0,
    CHECK_SECTION_ORDER,
    CHECK_WORD_COUNT_MIN,
    CHECK_WORD_COUNT_MAX,
    CHECK_REGEX_FORBIDDEN,
    CHECK_REGEX_REQUIRED,
    CHECK_HEADING_FORMAT,
    CHECK_LLM_SEMANTIC
} RuleCheckType;

/* Statut après évaluation */
typedef enum {
    RULE_STATUS_OK = 0,
    RULE_STATUS_WARNING,
    RULE_STATUS_ERROR,
    RULE_STATUS_PENDING   /* utilisé pour les règles LLM asynchrones */
} RuleStatus;


/* ============================================================
 * STRUCTURES
 * ============================================================ */

/* Représente UNE règle métier chargée depuis un fichier JSON */
typedef struct {
    char id[32];
    char category[32];
    char description[256];
    int severity;
    int check_type;
    char *parameter;
    char *target_section;
} Rule;


/* Résultat de l'évaluation d'une règle */
typedef struct {
    const Rule *rule;   /* règle évaluée */
    int passed;
    char message[256];
} RuleResult;


/* Ensemble de règles pour un type de document */
typedef struct {
    Rule *rules;
    size_t count;

    char document_type[64];   /* ex: "Mémoire de Licence" */
    char version[16];

} RuleSet;


/* Rapport global de conformité */
typedef struct {
    RuleResult *results;
    size_t count;
    size_t errors;
    size_t warnings;
    size_t infos;
} RuleReport;


/* ============================================================
 * API DU MOTEUR DE RÈGLES
 * ============================================================ */

/* Charge un fichier JSON de règles et retourne un RuleSet */
RuleSet* ruleset_load_from_file(const char *path);

/* Libère toute la mémoire associée à un RuleSet */
void ruleset_free(RuleSet *ruleset);

/* Évalue un document texte par rapport à un RuleSet */
RuleReport* rules_evaluate(
    const RuleSet *ruleset,
    const char *document_text
);

/* Libère un rapport de conformité */
void rule_report_free(RuleReport *report);

#endif /* INTELLIEDITOR_RULE_H */