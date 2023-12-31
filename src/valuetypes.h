#ifndef CRBOUT_INTVALUE_H
#define CRBOUT_INTVALUE_H

#include <stddef.h>
#include <stdint.h>

#define CRB_VALUE_MAX INT64_MAX
#define CRB_VALUE_MIN INT64_MIN

#define CRB_CHAR_MAX UINT32_MAX
#define CRB_CHAR_MIN 0

// Wtf linux
typedef long long int _int64_t;

typedef _int64_t crb_value_t;
typedef uint_least32_t char32_t;

#endif