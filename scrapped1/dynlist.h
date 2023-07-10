#ifndef DYNLIST_H
#define DYNLIST_H

#include "errors.h"
#include "stdint.h"

typedef struct {
    void* raw;
    size_t size;
} crb_dynlist_item_t;

typedef struct {
    crb_dynlist_item_t* items;
    unsigned int capacity;
    unsigned int length;
} crb_dynlist_t;

CRB_ERROR crb_dynlist_create(crb_dynlist_t* self, unsigned int capacity);
CRB_ERROR crb_dynlist_add(crb_dynlist_t* self, crb_dynlist_item_t item);
CRB_ERROR crb_dynlist_addCopy(crb_dynlist_t* self, crb_dynlist_item_t item);
CRB_ERROR crb_dynlist_remove(crb_dynlist_t* self, crb_dynlist_item_t item);
CRB_ERROR crb_dynlist_removePtr(crb_dynlist_t* self, const void* ptr);
CRB_ERROR crb_dynlist_removeAt(crb_dynlist_t* self, unsigned int index);
CRB_ERROR crb_dynlist_popFirst(crb_dynlist_t* self, crb_dynlist_item_t* result);
CRB_ERROR crb_dynlist_popLast(crb_dynlist_t* self, crb_dynlist_item_t* result);
CRB_ERROR crb_dynlist_insert(crb_dynlist_t* self, crb_dynlist_item_t item, unsigned int index);
CRB_ERROR crb_dynlist_insertCopy(crb_dynlist_t* self, crb_dynlist_item_t item, unsigned int index);
CRB_ERROR crb_dynlist_get(crb_dynlist_t* self, unsigned int index, crb_dynlist_item_t* result);
CRB_ERROR crb_dynlist_setCapacity(crb_dynlist_t* self, unsigned int capacity);

#endif