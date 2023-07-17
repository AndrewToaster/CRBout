#ifndef CRBOUT_RBSTATE_H
#define CRBOUT_RBSTATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stack.h"
#include "heap.h"
#include "valuetypes.h"
#include "playground.h"

typedef struct crb_pos {
    size_t x;
    size_t y;
} crb_pos_t;

typedef enum crb_dir {
    CRB_DIR_NONE = 0,
    CRB_DIR_UP = 1,
    CRB_DIR_DOWN = 1 << 1,
    CRB_DIR_LEFT = 1 << 2,
    CRB_DIR_RIGHT = 1 << 3,
    
    CRB_DIR_LEFTUP = CRB_DIR_UP | CRB_DIR_LEFT,
    CRB_DIR_RIGHTUP = CRB_DIR_UP | CRB_DIR_RIGHT,
    CRB_DIR_LEFTDOWN = CRB_DIR_DOWN | CRB_DIR_LEFT,
    CRB_DIR_RIGHTDOWN = CRB_DIR_DOWN | CRB_DIR_RIGHT,

    CRB_DIR_UPDOWN = CRB_DIR_UP | CRB_DIR_DOWN,
    CRB_DIR_LEFTRIGHT = CRB_DIR_LEFT | CRB_DIR_RIGHT,

    CRB_DIR_ALL = CRB_DIR_UP | CRB_DIR_DOWN | CRB_DIR_LEFT | CRB_DIR_RIGHT
} crb_dir_e;

typedef enum crb_mode {
    CRB_MODE_TRAVERSE,
    CRB_MODE_CONDTRAVERSE,
    CRB_MODE_COMPARE,
    CRB_MODE_OPERATION,
    CRB_MODE_FLAGS,
    CRB_MODE_STACK,
    CRB_MODE_HEAP,
    CRB_MODE_IO,
    CRB_MODE_PLAYGROUND
} crb_mode_e;

/**
 * @brief Enum represeting all the defined flags
 */
typedef enum crb_flags {
    // No value
    CRB_FLAGS_NONE = 0,
    // Indicates that the operation ran without error
    CRB_FLAGS_SUCCESS = 1 << 0,
    // Indicates the result of the operation, e.g. comparing values
    CRB_FLAGS_RESULT = 1 << 1,
    // Indicates that the operation couldn't complete due to not having enough values on the stack
    CRB_FLAGS_STACK_LOW = 1 << 2,
    // Indicates that the operation encountered an invalid value somewhere
    CRB_FLAGS_INVALID_VALUE = 1 << 3,
    // Indicates that a division by zero would occur
    CRB_FLAGS_DIVISION_BY_ZERO = 1 << 4,
    // Indicates that a result had a fractional part that was removed
    CRB_FLAGS_TRUNCATED = 1 << 5,
    // Indicates that the underlying value representation over- or underflowed
    CRB_FLAGS_OVER_UNDER_FLOW = 1 << 6,
    // Indicates an error with IO, usually malformed character values
    CRB_FLAGS_IO_ERROR = 1 << 7
} crb_flags_e;

typedef struct crb_state {
    bool stopped;
    crb_dir_e dir;
    crb_pos_t pos;
    crb_playground_t pg;
    crb_mode_e mode;
    crb_stack_t stack;
    crb_heap_t heap;
    crb_value_t flags;
    size_t pointer;
    FILE *out;
    FILE *in;
} crb_state_t;


CRB_ERROR crb_state_init();
CRB_ERROR crb_state_step(crb_state_t *self);

#endif