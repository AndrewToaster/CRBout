#define CRB_ERR_BREAK_ON_ERROR
#include "parser.h"
#include <stdlib.h>

#define _CRB_ERR_NEW(err_code, err_msg, err_off) CRB_ERR_NEWX(err_code, err_msg, "Offset: %llu", err_off)
#define _CRB_ERR_NEWX(err, err_off) CRB_ERR_NEWX((err.code), (err.message), "Offset: %llu", err_off)

crb_error_t crb_parser_peekChar(crb_parser_state_t *state, int *result)
{
    if (!state || !state->file) return crb_error_invalidPtr;
    
    int c;
    if (state->prev != CRB_PARSER_NO_CHAR) 
    { 
        c = state->prev;
    }
    else
    {
        c = fgetc(state->file);
        state->prev = c;
    }

    *result = c;

    return crb_error_ok;
}

crb_error_t crb_parser_readChar(crb_parser_state_t *state, int *result)
{
    if (!state || !state->file) return crb_error_invalidPtr;
    
    CRB_ERR_ASSERT_OK(crb_parser_peekChar(state, result));
    if (*result == EOF)
    {
        CRB_ERR_THROW(_CRB_ERR_NEWX(crb_error_eof, state->offset));
    }
    state->offset++;
    state->prev = CRB_PARSER_NO_CHAR;
    return crb_error_ok;
}

crb_error_t crb_parser_readDigit(crb_parser_state_t *state, int *result)
{
    CRB_ERR_ASSERT_OK(crb_parser_readChar(state, result));
    if (*result > '9' || *result < '0')
    {
        CRB_ERR_THROW(_CRB_ERR_NEW(CRB_ERR_INVALID_VALUE, "Expected DIGIT", state->offset));
    }
    return crb_error_ok;
}

crb_error_t crb_parser_eatChar(crb_parser_state_t *state, int letter)
{
    int c;
    CRB_ERR_ASSERT_OK(crb_parser_readChar(state, &c));
    if (c != letter)
    {
        CRB_ERR_THROW(_CRB_ERR_NEW(CRB_ERR_INVALID_VALUE, "Got a different character than expected", state->offset));
    }
    return crb_error_ok;
}

crb_error_t crb_parser_eatWhitespace(crb_parser_state_t *state)
{
    int c;
    CRB_ERR_ASSERT_OK(crb_parser_readChar(state, &c));
    if (c != ' ' && c != '\t')
    {
        CRB_ERR_THROW(_CRB_ERR_NEW(CRB_ERR_INVALID_VALUE, "Got a different character than expected", state->offset));
    }
    return crb_error_ok;
}

crb_error_t crb_parser_eatNewline(crb_parser_state_t *state)
{
    int c1, c2;
    CRB_ERR_ASSERT_OK(crb_parser_readChar(state, &c1));
    if (c1 != '\r' && c1 != '\n')
    {
        CRB_ERR_THROW(_CRB_ERR_NEW(CRB_ERR_INVALID_VALUE, "Got a different character than expected", state->offset));
    }
    
    CRB_ERR_ASSERT_OK(crb_parser_peekChar(state, &c2));
    if (c1 == '\r' &&  c2 == '\n')
    {
        state->offset++;
    }
    return crb_error_ok;
}