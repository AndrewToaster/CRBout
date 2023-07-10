#include "playground.h"
#include <stdlib.h>
#include <string.h>

crb_error_t crb_playground_create(size_t width, size_t height, crb_playground_t* result)
{
    if (!result) return crb_error_invalidPtr;

    char32_t** cols = calloc(height, sizeof(cols));
    if (!cols) return CRB_ERR__ENEW(CRB_ERR_ALLOC_FAILED, "Failed to allocate columns");

    for (size_t i = 0; i < height; i++)
    {
        char32_t* string = calloc(width, sizeof(string));
        if (!string)
        {
            // Pesky memory leak
            for (size_t j = 0; j < i; j++)
            {
                free(cols[j]);
            }
            free(cols);

            return CRB_ERR__ENEW(CRB_ERR_ALLOC_FAILED, "Failed to allocate row");
        }

        cols[i] = string;
    }

    *result = (crb_playground_t){
        .width = width,
        .height = height,
        .data = cols
    };

    return crb_error_ok;
}

crb_error_t crb_playground_write(crb_playground_t *self, size_t x, size_t y, char32_t data)
{
    if (!self) return crb_error_invalidPtr;
    self->data[y][x] = data;
    return crb_error_ok;
}

crb_error_t crb_playground_read(crb_playground_t *self, size_t x, size_t y, char32_t *result)
{
    if (!self || !result) return crb_error_invalidPtr;
    *result = self->data[y][x];
    return crb_error_ok;
}

crb_error_t crb_playground_resize(crb_playground_t *self, size_t newW, size_t newH)
{
    if (!self) return crb_error_invalidPtr;

    crb_playground_t new;
    crb_error_t err;
    if (crb_error_isFail(err = crb_playground_create(newW, newH, &new))) return err;
    
    for (size_t i = 0; i < self->height; i++)
    {
        if (!memcpy(new.data[i], self->data[i], sizeof(char32_t*) * self->width))
        {
            crb_playground_free(&new);
        }
    }

    return crb_error_ok;
}

void crb_playground_free(crb_playground_t* self)
{
    for (size_t i = 0; i < self->height; i++)
    {
        free(self->data[i]);
    }
    free(self->data);
}