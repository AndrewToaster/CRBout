#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#error Not yet implemented on windows
#endif

char* crb_util_formatString(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int length = vsnprintf(NULL, 0, format, ap) + 1;
    va_end(ap);
    if (length < 0) return NULL;

    char *msg = malloc(length);
    if (!msg) return NULL;

    va_start(ap, format);
    if (vsnprintf(msg, length, format, ap) < 0)
    {
        free(msg);
        return NULL;
    }
    va_end(ap);

    return msg;
}

char* crb_util_joinString(const char* left, const char* right, size_t maxlen)
{
    char *data = malloc(strnlen(left, maxlen) + strnlen(right, maxlen) + 1);
    if (!data) return NULL;
    strncat(data, left, maxlen);
    strncat(data, right, maxlen);
    return data;
}

void crb_util_sleep(unsigned int ms)
{
#if _WIN32
    Sleep(ms);
#endif
}