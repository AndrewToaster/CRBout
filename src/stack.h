#ifndef CRBOUT_STACK_H
#define CRBOUT_STACK_H

#include <stdint.h>
#include "valuetypes.h"
#include "errors.h"

#define CRB_STACK_DEFAULT_CAPACITY 16

typedef struct crb_stack {
    size_t top;
    size_t capacity;
    crb_value_t *raw;
} crb_stack_t;

CRB_ERROR crb_stack_make(crb_stack_t *self);
CRB_ERROR crb_stack_push(crb_stack_t *self, crb_value_t value);
CRB_ERROR crb_stack_pop(crb_stack_t *self, crb_value_t *result);
CRB_ERROR crb_stack_peek(crb_stack_t *self, crb_value_t *result);
CRB_ERROR crb_stack_copy(crb_stack_t *self);
void crb_stack_free(crb_stack_t *self);

#endif