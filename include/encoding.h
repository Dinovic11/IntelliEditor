#ifndef INTELLIEDITOR_ENCODING_H
#define INTELLIEDITOR_ENCODING_H

#include <stddef.h>
#include <stdbool.h>
#include <wchar.h>

bool utf8_to_utf16(const char *utf8, wchar_t *utf16, size_t utf16_size, size_t *written);
bool utf16_to_utf8(const wchar_t *utf16, char *utf8, size_t utf8_size, size_t *written);

wchar_t *utf8_to_utf16_alloc(const char *utf8);
char *utf16_to_utf8_alloc(const wchar_t *utf16);

bool enable_utf8_console(void);
void console_print(const char *text);
void console_printf(const char *format, ...);

#endif
