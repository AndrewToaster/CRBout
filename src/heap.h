#ifndef CRBOUT_HEAP_H
#define CRBOUT_HEAP_H

#include <stdint.h>
#include "valuetypes.h"
#include "errors.h"

#define CRB_HEAP_BUCKET_SIZE 1 << 14

typedef struct crb_heap_item {
    struct crb_heap_item *next;
    size_t key;
    crb_value_t value;
} crb_heap_item_t;

typedef struct crb_heap {
    size_t bucketSize;
    crb_heap_item_t **buckets;
} crb_heap_t;

CRB_ERROR crb_heap_make(crb_heap_t *self, size_t bucketSize);
CRB_ERROR crb_heap_set(crb_heap_t *self, size_t index, crb_value_t value);
CRB_ERROR crb_heap_get(crb_heap_t *self, size_t index, crb_value_t *result);
CRB_ERROR crb_heap_clear(crb_heap_t *self);
void crb_heap_free(crb_heap_t *self);

#endif