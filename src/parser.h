#ifndef CRBOUT_PARSER_H
#define CRBOUT_PARSER_H

#include <stdio.h>
#include <stddef.h>
#include "errors.h"
#include "playground.h"

#define CRB_PARSER_NO_CHAR -2
#define CRB_PARSER_NEW_STATE(arg_file) ((crb_parser_state_t) { .offset = 0, .prev = CRB_PARSER_NO_CHAR, .file = (arg_file) })

typedef struct crb_parser_state {
    size_t offset;
    int prev;
    FILE *file;
} crb_parser_state_t;

typedef struct crb_parser_error {
    size_t offset;
    const char* message;
} crb_parser_error_t;

CRB_ERROR crb_parser_peekChar(crb_parser_state_t *state, int *result);
CRB_ERROR crb_parser_readChar(crb_parser_state_t *state, int *result);
CRB_ERROR crb_parser_readDigit(crb_parser_state_t *state, int *result);
CRB_ERROR crb_parser_eatChar(crb_parser_state_t *state, int letter);
CRB_ERROR crb_parser_eatWhitespace(crb_parser_state_t *state);
CRB_ERROR crb_parser_eatNewline(crb_parser_state_t *state);

#endif