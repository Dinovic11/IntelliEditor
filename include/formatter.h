#ifndef INTELLIEDITOR_FORMATTER_H
#define INTELLIEDITOR_FORMATTER_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    STYLE_NONE = 0,
    STYLE_BOLD,
    STYLE_ITALIC,
    STYLE_UNDERLINE,
    STYLE_HEADING_1,
    STYLE_HEADING_2,
    STYLE_HEADING_3,
    STYLE_HEADING_4,
    STYLE_LIST_BULLET,
    STYLE_LIST_NUMBERED
} TextStyle;

typedef struct {
    size_t position;
    size_t length;
    TextStyle style;
} TextStyleRange;

typedef struct Formatter Formatter;

Formatter *formatter_create(void);
void formatter_destroy(Formatter *formatter);

bool formatter_set_style(Formatter *formatter, size_t position, size_t length, TextStyle style);
bool formatter_remove_style(Formatter *formatter, size_t position, size_t length, TextStyle style);
size_t formatter_style_count(const Formatter *formatter);
const TextStyleRange *formatter_get_styles(const Formatter *formatter);

char *formatter_render_markup(const Formatter *formatter, const char *text);

#endif
