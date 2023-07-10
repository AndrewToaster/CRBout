#include "stack.h"
#include <stdlib.h>

crb_error_t crb_stack_make(crb_stack_t *self)
{
    if (!self) return crb_error_invalidPtr;

    crb_value_t *stack = malloc(CRB_STACK_DEFAULT_CAPACITY * sizeof(stack));
    if (!stack) return crb_error_allocFailed;

    *self = (crb_stack_t) {
        .capacity = 16,
        .top = 0,
        .raw = stack
    };

    return crb_error_ok;
}

crb_error_t crb_stack_push(crb_stack_t *self, crb_value_t value)
{
    if (!self || !(self->raw)) return crb_error_invalidPtr;

    if (self->capacity == self->top)
    {
        crb_value_t *new = realloc(self->raw, sizeof(crb_value_t) * self->capacity * 2);
        if (!new) return crb_error_allocFailed;
    }

    self->raw[self->top++] = value;
    return crb_error_ok;
}

crb_error_t crb_stack_pop(crb_stack_t *self, crb_value_t *result)
{
    if (!self || !(self->raw)) return crb_error_invalidPtr;
    if (self->top == 0) return crb_error_outOfRange;

    if ((self->capacity / 4) == self->top)
    {
        crb_value_t *new = realloc(self->raw, sizeof(crb_value_t) * self->capacity / 2);
        if (new)
        {
            self->raw = new;
            self->capacity /= 2;
        }
        else
        {
            //? Technically this is a benign failure so no reason to throw
            //return crb_error_allocFailed;
        }
    }

    *result = self->raw[--self->top];
    return crb_error_ok;
}

crb_error_t crb_stack_peek(crb_stack_t *self, crb_value_t *result)
{
    if (!self || !(self->raw)) return crb_error_invalidPtr;
    if (self->top == 0) return crb_error_outOfRange;

    *result = self->raw[self->top - 1];
    return crb_error_ok;
}

crb_error_t crb_stack_copy(crb_stack_t *self)
{
    if (!self || !(self->raw)) return crb_error_invalidPtr;

    crb_error_t err;
    crb_value_t value;
    if (crb_error_isFail(err = crb_stack_peek(self, &value))) return err;
    if (crb_error_isFail(err = crb_stack_push(self, value))) return err;
    return crb_error_ok;
}

void crb_stack_free(crb_stack_t *self)
{
    if (!self || !(self->raw)) return;
    free(self->raw);
}