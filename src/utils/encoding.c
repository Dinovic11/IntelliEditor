#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "encoding.h"

bool utf8_to_utf16(const char *utf8, wchar_t *utf16, size_t utf16_size, size_t *written) {
    if (!utf8 || !utf16) {
        return false;
    }

    int required = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (required == 0) {
        return false;
    }

    if ((int)utf16_size < required) {
        if (written) {
            *written = (size_t)required;
        }
        return false;
    }

    int result = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, utf16, (int)utf16_size);
    if (result == 0) {
        return false;
    }

    if (written) {
        *written = (size_t)result;
    }
    return true;
}

bool utf16_to_utf8(const wchar_t *utf16, char *utf8, size_t utf8_size, size_t *written) {
    if (!utf16 || !utf8) {
        return false;
    }

    int required = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
    if (required == 0) {
        return false;
    }

    if ((int)utf8_size < required) {
        if (written) {
            *written = (size_t)required;
        }
        return false;
    }

    int result = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, utf8, (int)utf8_size, NULL, NULL);
    if (result == 0) {
        return false;
    }

    if (written) {
        *written = (size_t)result;
    }
    return true;
}

wchar_t *utf8_to_utf16_alloc(const char *utf8) {
    if (!utf8) {
        return NULL;
    }

    int required = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (required == 0) {
        return NULL;
    }

    wchar_t *buffer = malloc((size_t)required * sizeof(wchar_t));
    if (!buffer) {
        return NULL;
    }

    int result = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buffer, required);
    if (result == 0) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

char *utf16_to_utf8_alloc(const wchar_t *utf16) {
    if (!utf16) {
        return NULL;
    }

    int required = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
    if (required == 0) {
        return NULL;
    }

    char *buffer = malloc((size_t)required);
    if (!buffer) {
        return NULL;
    }

    int result = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, buffer, required, NULL, NULL);
    if (result == 0) {
        free(buffer);
        return NULL;
    }

    return buffer;
}
