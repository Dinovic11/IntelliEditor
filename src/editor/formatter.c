#include "formatter.h"
#include <stdlib.h>
#include <string.h>

struct Formatter {
    TextStyleRange *ranges;
    size_t count;
    size_t capacity;
};

static const char *style_begin_tag(TextStyle style) {
    switch (style) {
        case STYLE_BOLD: return "[B]";
        case STYLE_ITALIC: return "[I]";
        case STYLE_UNDERLINE: return "[U]";
        case STYLE_HEADING_1: return "[H1]";
        case STYLE_HEADING_2: return "[H2]";
        case STYLE_HEADING_3: return "[H3]";
        case STYLE_HEADING_4: return "[H4]";
        case STYLE_LIST_BULLET: return "[LI]";
        case STYLE_LIST_NUMBERED: return "[OL]";
        default: return "";
    }
}

static const char *style_end_tag(TextStyle style) {
    switch (style) {
        case STYLE_BOLD: return "[/B]";
        case STYLE_ITALIC: return "[/I]";
        case STYLE_UNDERLINE: return "[/U]";
        case STYLE_HEADING_1: return "[/H1]";
        case STYLE_HEADING_2: return "[/H2]";
        case STYLE_HEADING_3: return "[/H3]";
        case STYLE_HEADING_4: return "[/H4]";
        case STYLE_LIST_BULLET: return "[/LI]";
        case STYLE_LIST_NUMBERED: return "[/OL]";
        default: return "";
    }
}

static bool ensure_capacity(Formatter *formatter) {
    if (formatter->count < formatter->capacity) {
        return true;
    }
    size_t new_capacity = formatter->capacity ? formatter->capacity * 2 : 8;
    TextStyleRange *new_ranges = realloc(formatter->ranges, new_capacity * sizeof(TextStyleRange));
    if (!new_ranges) {
        return false;
    }
    formatter->ranges = new_ranges;
    formatter->capacity = new_capacity;
    return true;
}

Formatter *formatter_create(void) {
    Formatter *formatter = malloc(sizeof(Formatter));
    if (!formatter) {
        return NULL;
    }
    formatter->ranges = NULL;
    formatter->count = 0;
    formatter->capacity = 0;
    return formatter;
}

void formatter_destroy(Formatter *formatter) {
    if (!formatter) {
        return;
    }
    free(formatter->ranges);
    free(formatter);
}

bool formatter_set_style(Formatter *formatter, size_t position, size_t length, TextStyle style) {
    if (!formatter || style == STYLE_NONE || length == 0) {
        return false;
    }
    if (!ensure_capacity(formatter)) {
        return false;
    }
    formatter->ranges[formatter->count++] = (TextStyleRange){ position, length, style };
    return true;
}

bool formatter_remove_style(Formatter *formatter, size_t position, size_t length, TextStyle style) {
    if (!formatter || style == STYLE_NONE) {
        return false;
    }
    bool removed = false;
    for (size_t i = 0; i < formatter->count; ) {
        TextStyleRange *range = &formatter->ranges[i];
        if (range->style == style && range->position == position && range->length == length) {
            removed = true;
            if (i + 1 < formatter->count) {
                memmove(range, range + 1, (formatter->count - i - 1) * sizeof(TextStyleRange));
            }
            formatter->count -= 1;
        } else {
            i += 1;
        }
    }
    return removed;
}

size_t formatter_style_count(const Formatter *formatter) {
    return formatter ? formatter->count : 0;
}

const TextStyleRange *formatter_get_styles(const Formatter *formatter) {
    return formatter ? formatter->ranges : NULL;
}

static int range_compare(const void *a, const void *b) {
    const TextStyleRange *ra = a;
    const TextStyleRange *rb = b;
    if (ra->position < rb->position) return -1;
    if (ra->position > rb->position) return 1;
    if (ra->length < rb->length) return -1;
    if (ra->length > rb->length) return 1;
    return ra->style - rb->style;
}

char *formatter_render_markup(const Formatter *formatter, const char *text) {
    if (!text) {
        return NULL;
    }

    size_t text_len = strlen(text);
    size_t estimated_size = text_len + 256;
    char *output = malloc(estimated_size);
    if (!output) {
        return NULL;
    }

    TextStyleRange *sorted = NULL;
    size_t count = 0;
    if (formatter && formatter->count > 0) {
        count = formatter->count;
        sorted = malloc(count * sizeof(TextStyleRange));
        if (!sorted) {
            free(output);
            return NULL;
        }
        memcpy(sorted, formatter->ranges, count * sizeof(TextStyleRange));
        qsort(sorted, count, sizeof(TextStyleRange), range_compare);
    }

    size_t out_pos = 0;
    size_t text_pos = 0;
    for (size_t i = 0; i < count; ++i) {
        TextStyleRange range = sorted[i];
        if (range.position > text_len) {
            continue;
        }
        if (text_pos < range.position) {
            size_t chunk = range.position - text_pos;
            if (out_pos + chunk + 1 >= estimated_size) {
                estimated_size = (out_pos + chunk + 1) * 2;
                output = realloc(output, estimated_size);
            }
            memcpy(output + out_pos, text + text_pos, chunk);
            out_pos += chunk;
            text_pos = range.position;
        }

        const char *begin_tag = style_begin_tag(range.style);
        const char *end_tag = style_end_tag(range.style);
        size_t begin_len = strlen(begin_tag);
        size_t end_len = strlen(end_tag);
        if (out_pos + begin_len + range.length + end_len + 1 >= estimated_size) {
            estimated_size = (out_pos + begin_len + range.length + end_len + 1) * 2;
            output = realloc(output, estimated_size);
        }
        memcpy(output + out_pos, begin_tag, begin_len);
        out_pos += begin_len;
        size_t chunk = range.length;
        if (text_pos + chunk > text_len) {
            chunk = text_len - text_pos;
        }
        memcpy(output + out_pos, text + text_pos, chunk);
        out_pos += chunk;
        memcpy(output + out_pos, end_tag, end_len);
        out_pos += end_len;
        text_pos += chunk;
    }

    if (text_pos < text_len) {
        size_t chunk = text_len - text_pos;
        if (out_pos + chunk + 1 >= estimated_size) {
            estimated_size = out_pos + chunk + 1;
            output = realloc(output, estimated_size);
        }
        memcpy(output + out_pos, text + text_pos, chunk);
        out_pos += chunk;
    }

    output[out_pos] = '\0';
    free(sorted);
    return output;
}
