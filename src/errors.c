#include "errors.h"

crb_error_t crb_error_ok = (crb_error_t) {
    .code = CRB_ERR_OK,
    .message = (void*)0
};

crb_error_t crb_error_allocFailed = CRB_ERR_NEW(CRB_ERR_ALLOC_FAILED, "Failed to allocate resource"); /*(crb_error_t) {
    .code = CRB_ERR_ALLOC_FAILED,
    .message = "Failed to allocate resource"
};*/

crb_error_t crb_error_invalidPtr = CRB_ERR_NEW(CRB_ERR_INVALID_PTR, "Received an invalid pointer"); /*(crb_error_t) {
    .code = CRB_ERR_INVALID_PTR,
    .message = "Received an invalid pointer"
};*/

crb_error_t crb_error_outOfRange = CRB_ERR_NEW(CRB_ERR_OUT_OF_RANGE, "Value out of range"); /*(crb_error_t) {
    .code = CRB_ERR_OUT_OF_RANGE,
    .message = "Value out of range"
};*/

crb_error_t crb_error_invalidValue = CRB_ERR_NEW(CRB_ERR_INVALID_VALUE, "Received an invalid value"); /*(crb_error_t) {
    .code = CRB_ERR_INVALID_VALUE,
    .message = "Received an invalid value"
};*/

crb_error_t crb_error_invalidState = CRB_ERR_NEW(CRB_ERR_INVALID_STATE, "Encountered an invalid state"); /*(crb_error_t) {
    .code = CRB_ERR_INVALID_STATE,
    .message = "Encountered an invalid state"
};*/

crb_error_t crb_error_innerError = CRB_ERR_NEW(CRB_ERR_INNER_ERROR, "An internal error occured"); /*(crb_error_t) {
    .code = CRB_ERR_INNER_ERROR,
    .message = "An internal error occured"
};*/

crb_error_t crb_error_eof = CRB_ERR_NEW(CRB_ERR_EOF, "Encountered EOF");

crb_error_t CRB_ERR_LAST_ERROR = CRB_ERR_NEW(CRB_ERR_OK, (void*)0); /*(crb_error_t) {
    .code = CRB_ERR_OK,
    .message = (void*)0
};*/

int         CRB_ERR_LAST_ERROR_LINE = -1;
const char *CRB_ERR_LAST_ERROR_FUNC = "<unknown>";
const char *CRB_ERR_LAST_ERROR_FILE = "<unknown>";

bool crb_error_isOk(crb_error_t error)
{
    return error.code == crb_error_ok.code;
}

bool crb_error_isFail(crb_error_t error)
{
    return error.code != crb_error_ok.code;
}