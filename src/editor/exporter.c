#include "exporter.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

static bool write_all(FILE *stream, const void *data, size_t size) {
    return fwrite(data, 1, size, stream) == size;
}

static const char *style_to_rtf_open(TextStyle style) {
    switch (style) {
        case STYLE_BOLD: return "\\b ";
        case STYLE_ITALIC: return "\\i ";
        case STYLE_UNDERLINE: return "\\ul ";
        case STYLE_HEADING_1: return "\\pard\\fs36\\b ";
        case STYLE_HEADING_2: return "\\pard\\fs32\\b ";
        case STYLE_HEADING_3: return "\\pard\\fs28\\b ";
        case STYLE_HEADING_4: return "\\pard\\fs24\\b ";
        case STYLE_LIST_BULLET: return "\\pard\\fi-360\\li720\\bullet ";
        case STYLE_LIST_NUMBERED: return "\\pard\\fi-360\\li720\\pntext\\f0 1.\\tab ";
        default: return "";
    }
}

static const char *style_to_rtf_close(TextStyle style) {
    switch (style) {
        case STYLE_BOLD: return "\\b0 ";
        case STYLE_ITALIC: return "\\i0 ";
        case STYLE_UNDERLINE: return "\\ul0 ";
        case STYLE_HEADING_1:
        case STYLE_HEADING_2:
        case STYLE_HEADING_3:
        case STYLE_HEADING_4:
            return "\\b0 ";
        case STYLE_LIST_BULLET:
        case STYLE_LIST_NUMBERED:
            return "\\par ";
        default: return "";
    }
}

struct StyleEvent {
    size_t position;
    bool start;
    TextStyle style;
};

static int event_compare(const void *a, const void *b) {
    const struct StyleEvent *ea = a;
    const struct StyleEvent *eb = b;
    if (ea->position < eb->position) return -1;
    if (ea->position > eb->position) return 1;
    if (ea->start != eb->start) return eb->start - ea->start;
    return ea->style - eb->style;
}

static void append_rtf_char(FILE *stream, wchar_t wc) {
    if (wc == '\\' || wc == '{' || wc == '}') {
        fputc('\\', stream);
        fputc((char)wc, stream);
        return;
    }
    if (wc < 0x20 || wc == 0x7F) {
        fputc(' ', stream);
        return;
    }
    if (wc < 0x80) {
        fputc((char)wc, stream);
        return;
    }
    fprintf(stream, "\\u%d?", (int)wc);
}

static bool write_rtf_text(FILE *stream, const char *text) {
    wchar_t *wide = utf8_to_utf16_alloc(text);
    if (!wide) {
        return false;
    }
    for (size_t i = 0; wide[i] != L'\0'; ++i) {
        append_rtf_char(stream, wide[i]);
    }
    free(wide);
    return true;
}

bool exporter_save_text(const char *filename, const char *text) {
    if (!filename || !text) {
        return false;
    }
    FILE *stream = fopen(filename, "wb");
    if (!stream) {
        return false;
    }
    bool ok = write_all(stream, text, strlen(text));
    if (ok) {
        ok = write_all(stream, "\n", 1);
    }
    fclose(stream);
    return ok;
}

bool exporter_save_rtf(const char *filename, const char *text, const TextStyleRange *styles, size_t style_count) {
    if (!filename || !text) {
        return false;
    }
    FILE *stream = fopen(filename, "wb");
    if (!stream) {
        return false;
    }
    bool ok = true;
    fprintf(stream, "{\\rtf1\\ansi\\deff0\n");
    fprintf(stream, "{\\fonttbl{\\f0 Courier New;}}\n");
    fprintf(stream, "\\viewkind4\\uc1\n");

    struct StyleEvent {
        size_t position;
        bool start;
        TextStyle style;
    };

    size_t event_count = 0;
    struct StyleEvent *events = NULL;
    if (styles && style_count > 0) {
        events = malloc(style_count * 2 * sizeof(*events));
        if (!events) {
            fclose(stream);
            return false;
        }
        for (size_t i = 0; i < style_count; ++i) {
            events[event_count++] = (struct StyleEvent){ styles[i].position, true, styles[i].style };
            events[event_count++] = (struct StyleEvent){ styles[i].position + styles[i].length, false, styles[i].style };
        }
        qsort(events, event_count, sizeof(*events), event_compare);
    }

    size_t text_len = strlen(text);
    size_t text_pos = 0;
    for (size_t i = 0; i < event_count && ok; ++i) {
        size_t event_pos = events[i].position;
        if (event_pos > text_len) {
            continue;
        }
        if (text_pos < event_pos) {
            char segment[4096];
            size_t to_copy = event_pos - text_pos;
            while (to_copy > 0 && ok) {
                size_t chunk = to_copy < sizeof(segment) - 1 ? to_copy : sizeof(segment) - 1;
                memcpy(segment, text + text_pos, chunk);
                segment[chunk] = '\0';
                ok = write_rtf_text(stream, segment);
                text_pos += chunk;
                to_copy -= chunk;
            }
        }
        if (!ok) {
            break;
        }
        if (events[i].start) {
            fprintf(stream, "%s", style_to_rtf_open(events[i].style));
        } else {
            fprintf(stream, "%s", style_to_rtf_close(events[i].style));
        }
    }

    if (ok && text_pos < text_len) {
        ok = write_rtf_text(stream, text + text_pos);
    }
    if (ok) {
        fprintf(stream, "\n}");
    }

    free(events);
    fclose(stream);
    return ok;
}

bool exporter_save_ie(const char *filename, const char *text, const TextStyleRange *styles, size_t style_count) {
    if (!filename || !text) {
        return false;
    }
    FILE *stream = fopen(filename, "wb");
    if (!stream) {
        return false;
    }
    const char header[4] = {'I', 'E', 'F', 'M'};
    if (!write_all(stream, header, sizeof(header))) {
        fclose(stream);
        return false;
    }
    uint32_t version = 1;
    uint32_t text_len = (uint32_t)strlen(text);
    if (!write_all(stream, &version, sizeof(version)) || !write_all(stream, &text_len, sizeof(text_len))) {
        fclose(stream);
        return false;
    }
    if (!write_all(stream, text, text_len)) {
        fclose(stream);
        return false;
    }
    uint32_t count = (uint32_t)style_count;
    if (!write_all(stream, &count, sizeof(count))) {
        fclose(stream);
        return false;
    }
    for (size_t i = 0; i < style_count; ++i) {
        uint32_t position = (uint32_t)styles[i].position;
        uint32_t length = (uint32_t)styles[i].length;
        uint32_t style = (uint32_t)styles[i].style;
        if (!write_all(stream, &position, sizeof(position)) ||
            !write_all(stream, &length, sizeof(length)) ||
            !write_all(stream, &style, sizeof(style))) {
            fclose(stream);
            return false;
        }
    }
    fclose(stream);
    return true;
}

static int str_casecmp(const char *a, const char *b) {
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) {
            return ca - cb;
        }
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

bool exporter_save(const char *filename, const char *text, const TextStyleRange *styles, size_t style_count) {
    if (!filename || !text) {
        return false;
    }
    const char *ext = strrchr(filename, '.');
    if (!ext) {
        return false;
    }
    if (str_casecmp(ext, ".txt") == 0) {
        return exporter_save_text(filename, text);
    }
    if (str_casecmp(ext, ".rtf") == 0) {
        return exporter_save_rtf(filename, text, styles, style_count);
    }
    if (str_casecmp(ext, ".ie") == 0) {
        return exporter_save_ie(filename, text, styles, style_count);
    }
    return false;
}
