#ifndef CRBOUT_UTILS_H
#define CRBOUT_UTILS_H

#include <stdint.h>

#define CRB_UTIL_WITH(resource, code, res_free) \
    {\
        { code }\
        { res_free }\
    }
#define CRB_UTIL_FMT_BINU8 "%c%c%c%c%c%c%c%c"
#define CRB_UTIL_FMT_BINU16 CRB_UTIL_FMT_BINU8  CRB_UTIL_FMT_BINU8
#define CRB_UTIL_FMT_BINU32 CRB_UTIL_FMT_BINU16 CRB_UTIL_FMT_BINU16
#define CRB_UTIL_FMT_BINU64 CRB_UTIL_FMT_BINU32 CRB_UTIL_FMT_BINU32

#define CRB_UTIL_FMT_BINU8_VAL(byte) \
    ((byte) & (1 << 7) ? '1' : '0'), \
    ((byte) & (1 << 6) ? '1' : '0'), \
    ((byte) & (1 << 5) ? '1' : '0'), \
    ((byte) & (1 << 4) ? '1' : '0'), \
    ((byte) & (1 << 3) ? '1' : '0'), \
    ((byte) & (1 << 2) ? '1' : '0'), \
    ((byte) & (1 << 1) ? '1' : '0'), \
    ((byte) & (1 << 0) ? '1' : '0')
#define CRB_UTIL_FMT_BINU16_VAL(short) \
    CRB_UTIL_FMT_BINU8_VAL((short) >> 8), \
    CRB_UTIL_FMT_BINU8_VAL(short)

#define CRB_UTIL_FMT_BINU32_VAL(int) \
    CRB_UTIL_FMT_BINU16_VAL((int) >> 16), \
    CRB_UTIL_FMT_BINU16_VAL(int)

#define CRB_UTIL_FMT_BINU64_VAL(long) \
    CRB_UTIL_FMT_BINU32_VAL((long) >> 32), \
    CRB_UTIL_FMT_BINU32_VAL(long)

char* crb_util_formatString(const char* format, ...);
char* crb_util_joinString(const char* left, const char* right, size_t maxlen);
void crb_util_sleep(unsigned int ms);

#endif