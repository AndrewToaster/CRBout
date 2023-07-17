#ifndef CRBOUT_IO_H
#define CRBOUT_IO_H

#include "errors.h"
#include "valuetypes.h"
#include <stdio.h>

CRB_ERROR crb_io_initTerm();
CRB_ERROR crb_io_readChar(FILE *file, int *result);
CRB_ERROR crb_io_writeChar(FILE *file, char32_t result);
CRB_ERROR crb_io_any(FILE *file, bool *result);

#endif