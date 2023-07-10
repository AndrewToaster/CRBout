#ifndef CRBOUT_ERRORS_H
#define CRBOUT_ERRORS_H

#include <stdbool.h>
#include <stdint.h>
#include "pputil.h"
#include "utils.h"
#include "lib/portablesnips/debugtrap.h"

#define CRB_ERR_OK 0
#define CRB_ERR_ALLOC_FAILED 1
#define CRB_ERR_INVALID_PTR 2
#define CRB_ERR_OUT_OF_RANGE 3
#define CRB_ERR_INVALID_VALUE 4
#define CRB_ERR_INVALID_STATE 5
#define CRB_ERR_INNER_ERROR 6
#define CRB_ERR_EOF 7
#define CRB_ERR_PARSE_ERROR 8
#define CRB_ERR_WIN_ERROR 8
#define CRB_ERR_IO_ERROR 9

#ifdef CRB_ERR_BREAK_ON_ERROR
#define CRB_ERR__TRAP() psnip_trap()
#else
#define CRB_ERR__TRAP()
#endif

#define CRB_ERR__EISOK(expression) crb_error_isOk((CRB_ERR_LAST_ERROR = (expression)))
#define CRB_ERR__EISFAIL(expression) crb_error_isFail((CRB_ERR_LAST_ERROR = (expression)))
#define CRB_ERR_LAST_ERROR      crb_error_MACRO_ERROR_HOLDER_DO_NOT_USE
#define CRB_ERR_LAST_ERROR_LINE crb_error_MACRO_ERROR_LINE_HOLDER_DO_NOT_USE
#define CRB_ERR_LAST_ERROR_FUNC crb_error_MACRO_ERROR_FUNC_HOLDER_DO_NOT_USE
#define CRB_ERR_LAST_ERROR_FILE crb_error_MACRO_ERROR_FILE_HOLDER_DO_NOT_USE
#define CRB_ERR__ESETDIAG { CRB_ERR_LAST_ERROR_LINE = __LINE__; CRB_ERR_LAST_ERROR_FUNC = __func__; CRB_ERR_LAST_ERROR_FILE = __FILE__; }

#define CRB_ERR_NEW(err_code, err_text) ((crb_error_t) { .code = (err_code), .message = (err_text), .data = (void*)0 })
#define CRB_ERR_NEWX(err_code, err_text, err_fmt, args...) ((crb_error_t) { .code = (err_code), .message = (err_text), .data = crb_util_formatString((err_fmt), ##args) })

#define CRB_ERR_WITH_CODE(err, err_code) ({ crb_error_t poggies = (err); poggies.code = (err_code); poggies; })
#define CRB_ERR_WITH_MESSAGE(err, err_msg) ({ crb_error_t poggies = (err); poggies.message = (err_msg); poggies; })
#define CRB_ERR_WITH_DATA(err, err_fmt, args...) ({ crb_error_t poggies = (err); poggies.data = crb_util_formatString((err_fmt), ##args); poggies; })
#define CRB_ERR_WITHX_CODE(err, err_code) ({ err.code = (err_code); err; })
#define CRB_ERR_WITHX_MESSAGE(err, err_msg) ({ err.message = (err_msg); err; })
#define CRB_ERR_WITHX_DATA(err, err_fmt, args...) ({ err.data = crb_util_formatString((err_fmt), ##args); err; })

#define CRB_ERR_ASSERT_OK(expression) { if (CRB_ERR__EISFAIL(expression)) { CRB_ERR__TRAP(); return CRB_ERR_LAST_ERROR; } }
#define CRB_ERR_ASSERT_TRUE(expression, err) { if (!(expression)) { CRB_ERR__TRAP(); CRB_ERR__ESETDIAG; return (err); } }
#define CRB_ERR_ASSERT_FALSE(expression, err) { if (expression) { CRB_ERR__TRAP(); CRB_ERR__ESETDIAG; return (err); } }

#define CRB_ERR_ASSERT_OKX(expression, on_err) { if (CRB_ERR__EISFAIL(expression)) { CRB_ERR__TRAP(); { on_err } } }
#define CRB_ERR_ASSERT_TRUEX(expression, on_err) { if (!(expression)) { CRB_ERR__TRAP(); CRB_ERR__ESETDIAG; { on_err } } }
#define CRB_ERR_ASSERT_FALSEX(expression, on_err) { if (expression) { CRB_ERR__TRAP(); CRB_ERR__ESETDIAG; { on_err } } }

#define CRB_ERR_THROW(err) { CRB_ERR__TRAP(); CRB_ERR__ESETDIAG; return (err); }

#define CRB_ERROR __attribute__((warn_unused_result)) crb_error_t
#define CRB_ERR_FREE(err) free((err).data)

#define CRB_ERR_TRY(try_expr, err_name, err_block) CRB_ERR_ASSERT_OKX((try_expr), { crb_error_t (err_name) = CRB_ERR_LAST_ERROR; { err_block } })

typedef struct crb_error {
    ssize_t code;
    const char* message;
    char* data;
} crb_error_t;

extern crb_error_t crb_error_ok;
extern crb_error_t crb_error_invalidPtr;
extern crb_error_t crb_error_allocFailed;
extern crb_error_t crb_error_outOfRange;
extern crb_error_t crb_error_invalidValue;
extern crb_error_t crb_error_invalidState;
extern crb_error_t crb_error_innerError;
extern crb_error_t crb_error_eof;
extern crb_error_t CRB_ERR_LAST_ERROR;

extern int         CRB_ERR_LAST_ERROR_LINE;
extern const char *CRB_ERR_LAST_ERROR_FUNC;
extern const char *CRB_ERR_LAST_ERROR_FILE;

__attribute__((hot)) bool crb_error_isOk(crb_error_t error);
__attribute__((hot)) bool crb_error_isFail(crb_error_t error);

#endif