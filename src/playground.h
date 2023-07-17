#ifndef CRBOUT_PLAYGROUND_H
#define CRBOUT_PLAYGROUND_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "errors.h"

typedef uint_least32_t char32_t;

typedef struct crb_playground {
    size_t width;
    size_t height;
    char32_t **data;
} crb_playground_t;

CRB_ERROR crb_playground_make(crb_playground_t *self, size_t width, size_t height);
CRB_ERROR crb_playground_load(crb_playground_t *self, FILE *file);
CRB_ERROR crb_playground_set(crb_playground_t *self, size_t x, size_t y, char32_t value);
CRB_ERROR crb_playground_get(crb_playground_t *self, size_t x, size_t y, char32_t* result);
CRB_ERROR crb_playground_resize(crb_playground_t *self, size_t width, size_t height);
void crb_playground_free(crb_playground_t *self);

#endif