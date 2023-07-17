#include "playground.h"
#include "parser.h"
#include <stdlib.h>

crb_error_t crb_playground_make(crb_playground_t *self, size_t width, size_t height)
{
    if (!self) return crb_error_invalidPtr;

    char32_t **rows = malloc(height * sizeof(rows));
    if (!rows) return crb_error_allocFailed;

    for (size_t i = 0; i < height; i++)
    {
        char32_t *str = calloc(width, sizeof(str));
        if (!str) 
        {
            for (size_t j = 0; j < i; j++)
            {
                free(rows[j]);
            }
            return crb_error_allocFailed;
        }
        rows[i] = str;
    }
    
    *self = (crb_playground_t) {
        .height = height,
        .width = width,
        .data = rows
    };

    //printf("CREATED ARRAY\t-\tW:%llu\tH:%llu\n", width, height);

    return crb_error_ok;
}


crb_error_t crb_playground_load(crb_playground_t *self, FILE *file)
{
    if (!self) return crb_error_invalidPtr;
    if (!file) return crb_error_invalidPtr;
    
    char wBuf[21] = {0}, hBuf[21] = {0};
    size_t x = 0, y = 0, w = 0, h = 0;
    crb_parser_state_t state = CRB_PARSER_NEW_STATE(file);
    int c;

    CRB_ERR_ASSERT_OK(crb_parser_eatChar(&state, '/'));
    CRB_ERR_ASSERT_OK(crb_parser_eatChar(&state, '/'));

    for (size_t i = 0; i < 22; i++)
    {
        CRB_ERR_ASSERT_FALSE(i == 21, CRB_ERR_NEW(CRB_ERR_OUT_OF_RANGE, "Exceed the digit limit on width"));
        CRB_ERR_ASSERT_OK(crb_parser_peekChar(&state, &c));
        if (c == ',')
        {
            CRB_ERR_ASSERT_OK(crb_parser_eatChar(&state, ','));
            break;
        }
        CRB_ERR_ASSERT_OK(crb_parser_readDigit(&state, &c));
        //: This is valid since crb_parser_readDigit ensures it is a valid ASCII digit
        wBuf[i] = c;
    }
    for (size_t i = 0; i < 22; i++)
    {
        CRB_ERR_ASSERT_FALSE(i == 21, CRB_ERR_NEW(CRB_ERR_OUT_OF_RANGE, "Exceed the digit limit on height"));
        CRB_ERR_ASSERT_OK(crb_parser_peekChar(&state, &c));
        if (c == '\n' || c == '\r')
        {
            CRB_ERR_ASSERT_OK(crb_parser_eatNewline(&state));
            break;
        }
        CRB_ERR_ASSERT_OK(crb_parser_readDigit(&state, &c));
        //: This is valid since crb_parser_readDigit ensures it is a valid ASCII digit
        hBuf[i] = c;
    }
    //: Theoretically this should never fail, at least not based on input being malformed
    w = strtoull(wBuf, NULL, 10);
    h = strtoull(hBuf, NULL, 10);
    
    CRB_ERR_ASSERT_OK(crb_playground_make(self, w, h));

    while (true)
    {
        CRB_ERR_ASSERT_OK(crb_parser_peekChar(&state, &c));
        if (c == EOF)
        {
            break;
        }
        else if (c == '\n' || c == '\r' || x == w)
        {
            CRB_ERR_ASSERT_OK(crb_parser_eatNewline(&state));
            y++;
            x = 0;
            continue;
        }
        CRB_ERR_ASSERT_OK(crb_parser_readChar(&state, &c));
        CRB_ERR_ASSERT_OK(crb_playground_set(self, x, y, c));
        x++;
    }

    return crb_error_ok;
}

crb_error_t crb_playground_set(crb_playground_t *self, size_t x, size_t y, char32_t value)
{
    if (!self) return crb_error_invalidPtr;
    CRB_ERR_ASSERT_FALSE(x > self->width - 1 || y > self->height - 1, CRB_ERR_NEWX(CRB_ERR_OUT_OF_RANGE, "Index was out of bounds", "X:%zu\nY:%zu", x, y));

    //printf("SET CHARACTER: %u\t-\tX:%llu\tY:%llu\n", value, x, y);
    self->data[y][x] = value;
    return crb_error_ok;
}


__attribute__((hot))
crb_error_t crb_playground_get(crb_playground_t *self, size_t x, size_t y, char32_t *result)
{
    if (!self) return crb_error_invalidPtr;
    CRB_ERR_ASSERT_FALSE(x > self->width - 1 || y > self->height - 1, CRB_ERR_NEW(CRB_ERR_OUT_OF_RANGE, "Index was out of bounds"));

    *result = self->data[y][x];
    //printf("GET CHARACTER: %u\t-\tX:%llu\tY:%llu\n", *result, x, y);

    return crb_error_ok;
}

crb_error_t crb_playground_resize(crb_playground_t *self, size_t width, size_t height)
{
    CRB_ERR_ASSERT_TRUE(self, crb_error_invalidPtr);
    char32_t **rows = malloc(height * sizeof(rows));
    
    CRB_ERR_ASSERT_TRUE(self, crb_error_invalidPtr);
    if (!rows) return crb_error_allocFailed;

    for (size_t i = 0; i < height; i++)
    {
        char32_t *str = calloc(width, sizeof(str));
        if (!str) 
        {
            for (size_t j = 0; j < i; j++)
            {
                free(rows[j]);
            }
            return crb_error_allocFailed;
        }
        rows[i] = str;
    }
    
    *self = (crb_playground_t) {
        .height = height,
        .width = width,
        .data = rows
    };

    //printf("CREATED ARRAY\t-\tW:%llu\tH:%llu\n", width, height);

    return crb_error_ok;
    /*
    crb_playground_t new;
    CRB_ERR_ASSERT_OK(crb_playground_make(&new, width, height));

    for (size_t y = 0; y < self->height; y++)
    {
        crb_        
    }
    
    crb_playground_free(self);

    *self = new;

    return crb_error_ok;
    */
}

void crb_playground_free(crb_playground_t *self)
{
    if (!self || !(self->data)) return;

    for (size_t i = 0; i < self->height; i++)
    {
        free(self->data[i]);
    }
    free(self->data);

    self->data = NULL;
    self->width = 0;
    self->height = 0;
}