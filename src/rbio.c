#ifndef _WIN32
#error I can't be bothered to make a unix compatible terminal IO right now
#endif

#include <io.h>
#include <fcntl.h>
#include "rbio.h"
#include <conio.h>
#include <stdint.h>
#include <stdio.h>
#include <uchar.h>
#include <locale.h>
#include <windows.h>
#include "utils.h"

static mbstate_t state;

crb_error_t _any_term(FILE *_, bool *result)
{
    *result = kbhit();
    return crb_error_ok;
}

crb_error_t _any_file(FILE *file, bool *result)
{
    CRB_ERR_ASSERT_TRUE(file, CRB_ERR_NEW(CRB_ERR_INVALID_PTR, "Received a invalid file pointer"));
    int c = fgetc(file);
    if (c == EOF)
    {
        if (ferror(file)) CRB_ERR_THROW(CRB_ERR_NEW(CRB_ERR_IO_ERROR, strerror(errno)));
        *result = false;
    }
    else
    {
        *result = true;
        if (ungetc(c, file) == EOF) CRB_ERR_THROW(CRB_ERR_NEW(CRB_ERR_IO_ERROR, strerror(errno)));
    }
    return crb_error_ok;
}

crb_error_t _readChar_term(FILE *_, int *result)
{
    if (!kbhit())
        *result = -1;
    
    *result = getch();

    return crb_error_ok;
}

crb_error_t _readChar_file(FILE *file, int *result)
{
    int c;
    if ((c = fgetc(file)) == EOF && ferror(file))
    {
        CRB_ERR_THROW(CRB_ERR_NEW(CRB_ERR_IO_ERROR, strerror(errno)));
    }
    
    *result = c;
    return crb_error_ok;
}

typedef crb_error_t (*_any_func)(FILE *file, bool *result);
typedef crb_error_t (*_readChar_func)(FILE *file, int *result);

static _any_func _any[2] = {
    _any_term,
    _any_file
};
static _readChar_func _readChar[2] = {
    _readChar_term,
    _readChar_file
};

crb_error_t crb_io_initTerm()
{
    if (!SetConsoleCP(CP_UTF8) || !SetConsoleOutputCP(CP_UTF8)) CRB_ERR_THROW(CRB_ERR_NEW(CRB_ERR_WIN_ERROR, "Failed to set UTF-8 codepage"));
    return crb_error_ok;
}

crb_error_t crb_io_any(FILE *file, bool *result)
{
    return _any[!_isatty(fileno(file))](file, result);
}

crb_error_t crb_io_readChar(FILE *file, int *result)
{
    return _readChar[!_isatty(fileno(file))](file, result);
}

crb_error_t crb_io_writeChar(FILE *file, char32_t value)
{
    char str[5] = { 0 };
    int len;
    CRB_ERR_ASSERT_FALSE((len = c32rtomb(str, value, &state)) < 0, CRB_ERR_NEW(CRB_ERR_PARSE_ERROR, "Failed to convert character"));
    CRB_ERR_ASSERT_FALSE(fputs(str, file) == EOF, CRB_ERR_NEW(CRB_ERR_IO_ERROR, strerror(errno)));
    return crb_error_ok;
}