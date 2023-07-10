#include "heap.h"
#include <stdlib.h>

crb_error_t crb_heap_make(crb_heap_t *self, size_t bucketSize)
{
    if (!self) return crb_error_invalidPtr;
    
    crb_heap_item_t **items = calloc(bucketSize, sizeof(items));
    if (!items) return crb_error_allocFailed;

    *self = (crb_heap_t) {
        .buckets = items,
        .bucketSize = bucketSize
    };

    return crb_error_ok;
}
crb_error_t crb_heap_set(crb_heap_t *self, size_t index, crb_value_t value)
{
    if (!self || !(self->buckets)) return crb_error_invalidPtr;
    
    crb_heap_item_t *new = malloc(sizeof(new));
    if (!new) return crb_error_invalidPtr;
    new->next = NULL;
    new->value = value;
    new->key = index;
    
    crb_heap_item_t *item = self->buckets[index % self->bucketSize];
    if (!item)
    {
        self->buckets[index % self->bucketSize] = new;
    }
    else
    {
        for (; item->next; item = item->next);
        item->next = item;
    }

    return crb_error_ok;
}

crb_error_t crb_heap_get(crb_heap_t *self, size_t index, crb_value_t *result)
{
    if (!self || !(self->buckets)) return crb_error_invalidPtr;
    
    crb_heap_item_t *item = self->buckets[index % self->bucketSize];
    for (; item && item->key != index; item = item->next);
    *result = item ? item->value : 0;

    return crb_error_ok;
}

crb_error_t crb_heap_clear(crb_heap_t *self)
{
    if (!self || !(self->buckets)) return crb_error_invalidPtr;
    for (size_t i = 0; i < self->bucketSize; i++)
    {
        crb_heap_item_t *item = self->buckets[i];
        if (item)
        {
            crb_heap_item_t *next = item->next;
            while (next)
            {
                crb_heap_item_t *tmp = next;
                next = next->next;
                free(tmp);
            }
            free(item);
        }
    }
    return crb_error_ok;
}

void crb_heap_free(crb_heap_t *self)
{
    if (!self || !(self->buckets)) return;
    // Ignore error
    if (crb_heap_clear(self).code) {}
    free(self->buckets);
}