#ifndef ERRORS_H
#define ERRORS_H

#include <stdbool.h>

#define CRB_ERR_OK 0
#define CRB_ERR_ALLOC_FAILED 1
#define CRB_ERR_INVALID_PTR 2
#define CRB_ERR_OUT_OF_RANGE 3
#define CRB_ERR_INVALID_VALUE 4

#define CRB_ERR__ENEW2(err_code) ((crb_error_t) { .code = (err_code), .message = ((void*)0) })
#define CRB_ERR__ENEW(err_code, err_text) ((crb_error_t) { .code = (err_code), .message = (err_text) })

typedef struct {
    int code;
    const char* message;
} crb_error_t;

extern crb_error_t crb_error_ok;
extern crb_error_t crb_error_invalidPtr;
extern crb_error_t crb_error_allocFailed;
extern crb_error_t crb_error_outOfRange;
extern crb_error_t crb_error_invalidValue;

crb_error_t crb_error_create(int code, const char* message);
bool crb_error_isOk(crb_error_t error);
bool crb_error_isFail(crb_error_t error);

#endif