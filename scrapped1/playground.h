#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <stdint.h>
#include <uchar.h>
#include "errors.h"

typedef struct
{
    char32_t** data;
    size_t width;
    size_t height;
} crb_playground_t;

CRB_ERROR crb_playground_create(size_t width, size_t height, crb_playground_t* result);
CRB_ERROR crb_playground_write(crb_playground_t* self, size_t x, size_t y, char32_t data);
CRB_ERROR crb_playground_read(crb_playground_t* self, size_t x, size_t y, char32_t* result);
CRB_ERROR crb_playground_resize(crb_playground_t* self, size_t newW, size_t newH);
void crb_playground_free(crb_playground_t* self);



#endif