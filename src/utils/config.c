#include "config.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *section;
    char *key;
    char *value;
} ConfigEntry;

struct Config {
    ConfigEntry *entries;
    size_t count;
    size_t capacity;
};

static char *config_strdup(const char *source) {
    if (!source) {
        return NULL;
    }
    size_t len = strlen(source) + 1;
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, source, len);
    }
    return copy;
}

static char *config_trim(char *text) {
    if (!text) {
        return NULL;
    }
    char *start = text;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';
    if (start != text) {
        memmove(text, start, end - start + 1);
    }
    return text;
}

static bool config_ensure_capacity(Config *config) {
    if (config->count < config->capacity) {
        return true;
    }
    size_t new_capacity = config->capacity ? config->capacity * 2 : 8;
    ConfigEntry *new_entries = realloc(config->entries, new_capacity * sizeof(ConfigEntry));
    if (!new_entries) {
        return false;
    }
    config->entries = new_entries;
    config->capacity = new_capacity;
    return true;
}

static ConfigEntry *config_find_entry(const Config *config, const char *section, const char *key) {
    for (size_t i = 0; i < config->count; ++i) {
        if (strcmp(config->entries[i].section, section) == 0 && strcmp(config->entries[i].key, key) == 0) {
            return &config->entries[i];
        }
    }
    return NULL;
}

Config *config_create(void) {
    Config *config = malloc(sizeof(Config));
    if (!config) {
        return NULL;
    }
    config->entries = NULL;
    config->count = 0;
    config->capacity = 0;
    return config;
}

void config_destroy(Config *config) {
    if (!config) {
        return;
    }
    for (size_t i = 0; i < config->count; ++i) {
        free(config->entries[i].section);
        free(config->entries[i].key);
        free(config->entries[i].value);
    }
    free(config->entries);
    free(config);
}

bool config_set(Config *config, const char *section, const char *key, const char *value) {
    if (!config || !section || !key || !value) {
        return false;
    }

    ConfigEntry *entry = config_find_entry(config, section, key);
    if (entry) {
        char *new_value = config_strdup(value);
        if (!new_value) {
            return false;
        }
        free(entry->value);
        entry->value = new_value;
        return true;
    }

    if (!config_ensure_capacity(config)) {
        return false;
    }

    ConfigEntry *new_entry = &config->entries[config->count++];
    new_entry->section = config_strdup(section);
    new_entry->key = config_strdup(key);
    new_entry->value = config_strdup(value);
    if (!new_entry->section || !new_entry->key || !new_entry->value) {
        free(new_entry->section);
        free(new_entry->key);
        free(new_entry->value);
        return false;
    }
    return true;
}

const char *config_get(const Config *config, const char *section, const char *key, const char *default_value) {
    if (!config || !section || !key) {
        return default_value;
    }
    ConfigEntry *entry = config_find_entry(config, section, key);
    return entry ? entry->value : default_value;
}

bool config_remove(Config *config, const char *section, const char *key) {
    if (!config || !section || !key) {
        return false;
    }
    for (size_t i = 0; i < config->count; ++i) {
        if (strcmp(config->entries[i].section, section) == 0 && strcmp(config->entries[i].key, key) == 0) {
            free(config->entries[i].section);
            free(config->entries[i].key);
            free(config->entries[i].value);
            if (i + 1 < config->count) {
                memmove(&config->entries[i], &config->entries[i + 1], (config->count - i - 1) * sizeof(ConfigEntry));
            }
            config->count -= 1;
            return true;
        }
    }
    return false;
}

bool config_load(Config *config, const char *path) {
    if (!config || !path) {
        return false;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return false;
    }

    char line[1024];
    char section[256] = "default";

    while (fgets(line, sizeof(line), file)) {
        char *text = config_trim(line);
        if (!*text || text[0] == ';' || text[0] == '#') {
            continue;
        }
        if (text[0] == '[') {
            char *end = strchr(text, ']');
            if (!end) {
                continue;
            }
            *end = '\0';
            strncpy(section, text + 1, sizeof(section) - 1);
            section[sizeof(section) - 1] = '\0';
            config_trim(section);
            continue;
        }
        char *equals = strchr(text, '=');
        if (!equals) {
            continue;
        }
        *equals = '\0';
        char *key = config_trim(text);
        char *value = config_trim(equals + 1);
        if (!key || !value) {
            continue;
        }
        if (!config_set(config, section, key, value)) {
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

static int config_compare_entries(const void *a, const void *b) {
    const ConfigEntry *ea = a;
    const ConfigEntry *eb = b;
    int section_cmp = strcmp(ea->section, eb->section);
    if (section_cmp != 0) {
        return section_cmp;
    }
    return strcmp(ea->key, eb->key);
}

bool config_save(const Config *config, const char *path) {
    if (!config || !path) {
        return false;
    }

    ConfigEntry *sorted = malloc(config->count * sizeof(ConfigEntry));
    if (!sorted) {
        return false;
    }
    memcpy(sorted, config->entries, config->count * sizeof(ConfigEntry));
    qsort(sorted, config->count, sizeof(ConfigEntry), config_compare_entries);

    FILE *file = fopen(path, "w");
    if (!file) {
        free(sorted);
        return false;
    }

    const char *current_section = NULL;
    for (size_t i = 0; i < config->count; ++i) {
        const ConfigEntry *entry = &sorted[i];
        if (!current_section || strcmp(current_section, entry->section) != 0) {
            if (current_section) {
                fprintf(file, "\n");
            }
            fprintf(file, "[%s]\n", entry->section);
            current_section = entry->section;
        }
        fprintf(file, "%s=%s\n", entry->key, entry->value);
    }

    fclose(file);
    free(sorted);
    return true;
}
