#include "dynlist.h"
#include <stdlib.h>
#include <string.h>

crb_error_t crb_dynlist_create(crb_dynlist_t* self, unsigned int capacity)
{
    if (capacity < 0) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Capacity must be a non-negative integer");

    crb_dynlist_item_t* items = calloc(capacity, sizeof(crb_dynlist_item_t));
    if (!items) return CRB_ERR__ENEW(CRB_ERR_ALLOC_FAILED, "Failed to allocate items");

    *self = (crb_dynlist_t) {
        .items = items,
        .capacity = capacity,
        .length = 0
    };

    return crb_error_ok;
}

crb_error_t crb_dynlist_add(crb_dynlist_t* self, crb_dynlist_item_t item)
{
    if (!self) return crb_error_invalidPtr;
    
    if (self->capacity == self->length)
    {
        crb_dynlist_setCapacity(self, self->capacity == 0 
                                    ? 8 
                                    : self->capacity * 2);
    }

    self->items[self->length++] = item;

    return crb_error_ok;
}

crb_error_t crb_dynlist_remove(crb_dynlist_t* self, crb_dynlist_item_t item)
{
    return crb_dynlist_removePtr(self, item.raw);
}

crb_error_t crb_dynlist_removeAt(crb_dynlist_t* self, unsigned int index)
{
    if (index < 0) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Index must be a non-negative integer");
    if (index > self->length - 1) CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Index must be within the bounds of the list");

    if (self->length != index + 1)
    {
        memmove((self->items + index), (self->items + index + 1), sizeof(crb_dynlist_item_t) * (self->length - index));
    }
    self->length--;

    return crb_error_ok;
}

crb_error_t crb_dynlist_insert(crb_dynlist_t* self, crb_dynlist_item_t item, unsigned int index)
{
    if (index < 0) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Index must be a non-negative integer");
    // Don't subtract one since you can append behind the list
    // For example list a = [0, 1]
    // you can insert into index 0, 1, but also 2 (since that is just appending)
    if (index > self->length) CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Index must be within the bounds of the list");

    if (self->length == self->capacity)
    {
        crb_dynlist_setCapacity(self, self->capacity == 0 
                                    ? 8 
                                    : self->capacity * 2);
    }

    if (self->length != index)
    {
        memmove((self->items + index + 1), (self->items + index), sizeof(crb_dynlist_item_t) * (self->length - index));
    }
    self->items[index] = item;
    self->length++;

    return crb_error_ok;
}

crb_error_t crb_dynlist_get(crb_dynlist_t* self, unsigned int index, crb_dynlist_item_t* result)
{
    if (index < 0) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Index must be a non-negative integer");
    if (index > self->length - 1) CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Index must be within the bounds of the list");
    *result = self->items[index];
    return crb_error_ok;
}

crb_error_t crb_dynlist_setCapacity(crb_dynlist_t* self, unsigned int capacity)
{
    if (!self) return crb_error_invalidPtr;
    //if (capacity < 0) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Capacity must be a non-negative integer");

    crb_dynlist_item_t* items = realloc(self->items, self->capacity * sizeof(crb_dynlist_item_t));
    if (!items) return CRB_ERR__ENEW(CRB_ERR_ALLOC_FAILED, "Failed to resize array");

    self->items = items;
    if (self->capacity > capacity) 
        self->length = capacity;
    self->capacity = capacity;

    return crb_error_ok;
}

crb_error_t crb_dynlist_addCopy(crb_dynlist_t* self, crb_dynlist_item_t item)
{
    //if (item.size < 1) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Item size must be larger than 0");
    if (item.raw == NULL) return CRB_ERR__ENEW(CRB_ERR_INVALID_PTR, "Item raw pointer musn't be null");

    void* value = malloc(item.size);
    memcpy(value, item.raw, item.size);
    item.raw = value;

    crb_error_t err;
    if (crb_error_isFail(err = crb_dynlist_add(self, item)))
    {
        free(value);
    }

    return err;
}

crb_error_t crb_dynlist_removePtr(crb_dynlist_t* self, const void* ptr)
{
    if (self->length == 0) return crb_error_ok;

    for (size_t i = 0; i < self->length; i++)
    {
        if (self->items[i].raw != ptr)
            continue;
        
        return crb_dynlist_removeAt(self, i);
    }

    return crb_error_ok;
}

crb_error_t crb_dynlist_insertCopy(crb_dynlist_t* self, crb_dynlist_item_t item, unsigned int index)
{
    if (item.size < 1) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "Item size must be larger than 0");
    if (item.raw == NULL) return CRB_ERR__ENEW(CRB_ERR_INVALID_PTR, "Item raw pointer musn't be null");

    void* value = malloc(item.size);
    memcpy(value, item.raw, item.size);
    item.raw = value;

    crb_error_t err;
    if (crb_error_isFail(err = crb_dynlist_insert(self, item, index)))
    {
        free(value);
    }

    return err;
}

crb_error_t crb_dynlist_popFirst(crb_dynlist_t* self, crb_dynlist_item_t* result)
{
    if (!self) return crb_error_invalidPtr;
    if (self->length < 1) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "List must contain at least one value");

    crb_error_t err;
    if (crb_error_isFail(err = crb_dynlist_get(self, 0, result))) return err;
    if (crb_error_isFail(err = crb_dynlist_removeAt(self, 0))) return err;
    return crb_error_ok;
}

crb_error_t crb_dynlist_popLast(crb_dynlist_t* self, crb_dynlist_item_t* result)
{
    if (!self) return crb_error_invalidPtr;
    if (self->length < 1) return CRB_ERR__ENEW(CRB_ERR_OUT_OF_RANGE, "List must contain at least one value");

    crb_error_t err;
    if (crb_error_isFail(err = crb_dynlist_get(self, self->length - 1, result))) return err;
    if (crb_error_isFail(err = crb_dynlist_removeAt(self, self->length - 1))) return err;
    return crb_error_ok;
}