#ifndef INTELLIEDITOR_CONFIG_H
#define INTELLIEDITOR_CONFIG_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Config Config;

Config *config_create(void);
void config_destroy(Config *config);

bool config_load(Config *config, const char *path);
bool config_save(const Config *config, const char *path);

const char *config_get(const Config *config, const char *section, const char *key, const char *default_value);
bool config_set(Config *config, const char *section, const char *key, const char *value);
bool config_remove(Config *config, const char *section, const char *key);

#endif
