#ifndef INTELLIEDITOR_EXPORTER_H
#define INTELLIEDITOR_EXPORTER_H

#include <stddef.h>
#include <stdbool.h>
#include "formatter.h"

typedef enum {
    EXPORT_FORMAT_TXT,
    EXPORT_FORMAT_RTF,
    EXPORT_FORMAT_IE
} ExportFormat;

bool exporter_save_text(const char *filename, const char *text);
bool exporter_save_rtf(const char *filename, const char *text, const TextStyleRange *styles, size_t style_count);
bool exporter_save_ie(const char *filename, const char *text, const TextStyleRange *styles, size_t style_count);
bool exporter_save(const char *filename, const char *text, const TextStyleRange *styles, size_t style_count);

#endif // INTELLIEDITOR_EXPORTER_H
