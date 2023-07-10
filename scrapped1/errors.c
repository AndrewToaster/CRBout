#include "errors.h"

crb_error_t crb_error_ok = (crb_error_t) {
    .code = CRB_ERR_OK,
    .message = (void*)0
};

crb_error_t crb_error_allocFailed = (crb_error_t) {
    .code = CRB_ERR_ALLOC_FAILED,
    .message = "Failed to allocate resource"
};

crb_error_t crb_error_invalidPtr = (crb_error_t) {
    .code = CRB_ERR_INVALID_PTR,
    .message = "Received an invalid pointer"
};

crb_error_t crb_error_outOfRange = (crb_error_t) {
    .code = CRB_ERR_OUT_OF_RANGE,
    .message = "Value out of range"
};

crb_error_t crb_error_invalidValue = (crb_error_t) {
    .code = CRB_ERR_INVALID_VALUE,
    .message = "Received an invalid value"
};

crb_error_t crb_error_create(int code, const char* message)
{
    return (crb_error_t) {
        .code = code,
        .message = message
    };
}

bool crb_error_isOk(crb_error_t error)
{
    return error.code == crb_error_ok.code;
}

bool crb_error_isFail(crb_error_t error)
{
    return error.code != crb_error_ok.code;
}