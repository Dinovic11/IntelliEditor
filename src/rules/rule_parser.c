#include "rule_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON/cJSON.h"
#define cJSON_IsString(item) ((item) != NULL && (item)->type == cJSON_String)
#define cJSON_IsArray(item) ((item) != NULL && (item)->type == cJSON_Array)

/* ------------------------------------------------------------
 * Utilitaire : lire un fichier entier en mémoire
 * ------------------------------------------------------------
 */
static char* read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

/* ------------------------------------------------------------
 * Validation structurelle d'une règle
 * ------------------------------------------------------------
 */
int rule_parser_validate_rule(const Rule *rule) {
    if (!rule) return 0;

    if (rule->id[0] == '\0') return 0;
    if (rule->description[0] == '\0') return 0;

    if (rule->check_type < CHECK_SECTION_EXISTS ||
        rule->check_type > CHECK_LLM_SEMANTIC)
        return 0;

    return 1;
}

/* ------------------------------------------------------------
 * Libération mémoire d'un RuleSet
 * ------------------------------------------------------------
 */
void rule_parser_free(RuleSet *ruleset) {
    if (!ruleset) return;

    for (size_t i = 0; i < ruleset->count; ++i) {
        free(ruleset->rules[i].parameter);
        free(ruleset->rules[i].target_section);
    }

    free(ruleset->rules);
    free(ruleset);
}

/* ------------------------------------------------------------
 * Mapping string → enum RuleSeverity
 * ------------------------------------------------------------
 */
static RuleSeverity parse_severity(const char *s) {
    if (strcmp(s, "info") == 0) return RULE_SEVERITY_INFO;
    if (strcmp(s, "warning") == 0) return RULE_SEVERITY_WARNING;
    return RULE_SEVERITY_ERROR;
}

/* ------------------------------------------------------------
 * Mapping string → enum RuleCheckType
 * ------------------------------------------------------------
 */
static RuleCheckType parse_check_type(const char *s) {
    if (strcmp(s, "section_exists") == 0) return CHECK_SECTION_EXISTS;
    if (strcmp(s, "section_order") == 0) return CHECK_SECTION_ORDER;
    if (strcmp(s, "word_count_min") == 0) return CHECK_WORD_COUNT_MIN;
    if (strcmp(s, "word_count_max") == 0) return CHECK_WORD_COUNT_MAX;
    if (strcmp(s, "regex_forbidden") == 0) return CHECK_REGEX_FORBIDDEN;
    if (strcmp(s, "regex_required") == 0) return CHECK_REGEX_REQUIRED;
    if (strcmp(s, "heading_format") == 0) return CHECK_HEADING_FORMAT;
    return CHECK_LLM_SEMANTIC;
}

/* ------------------------------------------------------------
 * Chargement du fichier JSON de règles
 * ------------------------------------------------------------
 */
RuleSet* rule_parser_load(const char *path) {
    char *json_text = read_file(path);
    if (!json_text) return NULL;

    cJSON *root = cJSON_Parse(json_text);
    free(json_text);

    if (!root) return NULL;

    RuleSet *ruleset = (RuleSet*)calloc(1, sizeof(RuleSet));
    if (!ruleset) {
        cJSON_Delete(root);
        return NULL;
    }

    /* ---- META ---- */
    cJSON *meta = cJSON_GetObjectItem(root, "meta");
    if (meta) {
        cJSON *doc = cJSON_GetObjectItem(meta, "document_type");
        cJSON *ver = cJSON_GetObjectItem(meta, "version");

        if (cJSON_IsString(doc))
            strncpy(ruleset->document_type, doc->valuestring,
                    sizeof(ruleset->document_type) - 1);

        if (cJSON_IsString(ver))
            strncpy(ruleset->version, ver->valuestring,
                    sizeof(ruleset->version) - 1);
    }

    /* ---- RULES ---- */
    cJSON *rules_array = cJSON_GetObjectItem(root, "rules");
    if (!cJSON_IsArray(rules_array)) {
        rule_parser_free(ruleset);
        cJSON_Delete(root);
        return NULL;
    }

    size_t count = cJSON_GetArraySize(rules_array);
    ruleset->rules = (Rule*)calloc(count, sizeof(Rule));
    ruleset->count = count;

    for (size_t i = 0; i < count; ++i) {
        cJSON *jr = cJSON_GetArrayItem(rules_array, i);
        Rule *r = &ruleset->rules[i];

        cJSON *id = cJSON_GetObjectItem(jr, "id");
        cJSON *cat = cJSON_GetObjectItem(jr, "category");
        cJSON *desc = cJSON_GetObjectItem(jr, "description");
        cJSON *sev = cJSON_GetObjectItem(jr, "severity");
        cJSON *type = cJSON_GetObjectItem(jr, "check_type");
        cJSON *param = cJSON_GetObjectItem(jr, "parameter");
        cJSON *target = cJSON_GetObjectItem(jr, "target_section");

        if (cJSON_IsString(id))
            strncpy(r->id, id->valuestring, sizeof(r->id) - 1);

        if (cJSON_IsString(cat))
            strncpy(r->category, cat->valuestring,
                    sizeof(r->category) - 1);

        if (cJSON_IsString(desc))
            strncpy(r->description, desc->valuestring,
                    sizeof(r->description) - 1);

        if (cJSON_IsString(sev))
            r->severity = parse_severity(sev->valuestring);

        if (cJSON_IsString(type))
            r->check_type = parse_check_type(type->valuestring);

        if (param)
            r->parameter = cJSON_PrintUnformatted(param);

        if (cJSON_IsString(target))
            r->target_section = strdup(target->valuestring);

        if (!rule_parser_validate_rule(r)) {
            rule_parser_free(ruleset);
            cJSON_Delete(root);
            return NULL;
        }
    }

    cJSON_Delete(root);
    return ruleset;
}