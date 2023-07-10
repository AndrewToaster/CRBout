#include "rbstate.h"
#include "utils.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define FN(x) static void FNREF(x) (crb_state_t* self)
#define START_OP { set_flag(self, CRB_FLAG_OP_FAILED, true); }
#define END_OP { set_flag(self, CRB_FLAG_OP_FAILED, false); }
#define FNREF(x) crb_op_ ## x
#define _NUMARGS_USES_FLAGS(...)  (sizeof((crb_flags_e[]){__VA_ARGS__})/sizeof(crb_flags_e))
#define USES_FLAGS(...) _operates_on_flags(_NUMARGS_USES_FLAGS(__VA_ARGS__), __VA_ARGS__)


typedef int64_t ival;


static void set_result(crb_state_t* self, bool value)
{
    set_flag(self, CRB_FLAG_RESULT, value);
}

static void set_failed(crb_state_t* self, bool value)
{
    set_flag(self, CRB_FLAG_OP_FAILED, value);
}

static void set_flag(crb_state_t* self, crb_flags_e flag, bool value)
{
    if (value)
    {
        self->flags |= flag;
    }
    else
    {
        self->flags &= ~flag;
    }
}

static ival stack_pop(crb_state_t *self)
{
    crb_dynlist_item_t item;
    if (crb_error_isFail(crb_dynlist_popLast(&(self->stack), &item))) return 0;
    ival value = *((ival*)item.raw);
    free(item.raw);
    return value;
}

static void _operates_on_flags(crb_state_t *self, unsigned int amount, ...)
{
    va_list ap;
    va_start(ap, amount);
    for (unsigned int i = 0; i < amount; i++)
    {
        set_flag(self, va_arg(ap, crb_flags_e), false);
    }
}

typedef void (*crb_op_fn)(crb_state_t*);
static crb_op_fn FN_TABLE[9][CRB_NORM_CHAR('~')];

FN(CRB_OP_SET_CONDITIONALTRAVERSAL)
{
    self->mode = CRB_MODE_CONDTRAVERSAL;
}

FN(CRB_OP_SET_COMPARE)
{
    self->mode = CRB_MODE_COMPARISON;
}

FN(CRB_OP_SET_FLAGS)
{
    self->mode = CRB_MODE_FLAGS;
}

FN(CRB_OP_SET_OPERATION)
{
    self->mode = CRB_MODE_OPERATION;
}

FN(CRB_OP_SET_STACK)
{
    self->mode = CRB_MODE_STACK;
}

FN(CRB_OP_SET_HEAP)
{
    self->mode = CRB_MODE_HEAP;
}

FN(CRB_OP_SET_IO)
{
    self->mode = CRB_MODE_IO;
}

FN(CRB_OP_SET_MAP)
{
    self->mode = CRB_MODE_PLAYGROUND;
}

FN(CRB_OP_FLOW_RIGHT)
{
    self->dir = CRB_DIR_RIGHT;
}

FN(CRB_OP_FLOW_LEFT)
{
    self->dir = CRB_DIR_LEFT;    
}

FN(CRB_OP_FLOW_DOWN)
{
    self->dir = CRB_DIR_DOWN;    
}

FN(CRB_OP_FLOW_UP)
{
    self->dir = CRB_DIR_UP;
}

FN(CRB_OP_FLOW_FSREFLECT)
{
    if (self->dir == CRB_DIR_RIGHT || self->dir == CRB_DIR_UP || self->dir == CRB_DIR_RIGHTUP)
    {
        self->dir = CRB_DIR_RIGHTUP;
    }
    else if (self->dir == CRB_DIR_LEFT || self->dir == CRB_DIR_DOWN || self->dir == CRB_DIR_LEFTDOWN)
    {
        self->dir = CRB_DIR_LEFTDOWN;
    }
    else
    {
        self->dir ^= CRB_DIR_ALL;
    }
}

FN(CRB_OP_FLOW_BSREFLECT)
{
    if (self->dir == CRB_DIR_LEFT || self->dir == CRB_DIR_UP || self->dir == CRB_DIR_LEFTUP)
    {
        self->dir = CRB_DIR_LEFTUP;
    }
    else if (self->dir == CRB_DIR_RIGHT || self->dir == CRB_DIR_DOWN || self->dir == CRB_DIR_RIGHTDOWN)
    {
        self->dir = CRB_DIR_RIGHTDOWN;
    }
    else
    {
        self->dir ^= CRB_DIR_ALL;
    }
}

FN(CRB_OP_FLOW_VREFLECT)
{
    if (self->dir & CRB_DIR_UPDOWN)
    {
        self->dir &= CRB_DIR_UPDOWN;
    }
    else
    {
        self->dir ^= CRB_DIR_LEFTRIGHT;
    }
}

FN(CRB_OP_FLOW_HREFLECT)
{
    if (self->dir & CRB_DIR_LEFTRIGHT)
    {
        self->dir &= CRB_DIR_LEFTRIGHT;
    }
    else
    {
        self->dir ^= CRB_DIR_UPDOWN;
    }
}

FN(CRB_OP_FLOW_PREFLECT)
{
    self->dir = 1 >> (crb_util_randbyte() % 4);
}

FN(CRB_OP_FLOW_XREFLECT)
{
    self->dir = 1 >> (crb_util_randbyte() % 2) | 1 >> (2 + (crb_util_randbyte() % 2));
}

FN(CRB_OP_FLOW_SREFLECT)
{
    self->dir = crb_util_randbyte() % (1 << 4);
}

FN(CRB_OP_COMPARE_GREATERTHAN)
{
    USES_FLAGS(CRB_FLAG_LOW_STACK);
    START_OP
    if (self->stack.length < 2) { set_flag(self, CRB_FLAG_LOW_STACK, true); return; }
    ival a = stack_pop(self);
    ival b = stack_pop(self);
    set_result(self, a > b);
    END_OP
}

FN(CRB_OP_COMPARE_LESSTHAN)
{
    USES_FLAGS(CRB_FLAG_LOW_STACK);
    START_OP
    if (self->stack.length < 2) { set_flag(self, CRB_FLAG_LOW_STACK, true); return; }
    ival a = stack_pop(self);
    ival b = stack_pop(self);
    set_result(self, a < b);
    END_OP
}

FN(CRB_OP_COMPARE_EQUAL)
{
    USES_FLAGS(CRB_FLAG_LOW_STACK);
    START_OP
    if (self->stack.length < 2) { set_flag(self, CRB_FLAG_LOW_STACK, true); return; }
    ival a = stack_pop(self);
    ival b = stack_pop(self);
    set_result(self, a == b);
    END_OP
}

FN(CRB_OP_COMPARE_NOTEQUAL)
{
    USES_FLAGS(CRB_FLAG_LOW_STACK);
    START_OP
    if (self->stack.length < 2) { set_flag(self, CRB_FLAG_LOW_STACK, true); return; }
    ival a = stack_pop(self);
    ival b = stack_pop(self);
    set_result(self, a != b);
    END_OP
}

FN(CRB_OP_FLAGS_TRUE)
{
    USES_FLAGS(CRB_FLAG_LOW_STACK, CRB_FLAG_INVALID_VALUE);
    START_OP
    if (self->stack.length < 1) { set_flag(self, CRB_FLAG_LOW_STACK, true); return; }
    ival a = stack_pop(self);
    if (a < 0) 
    set_result(self, a > b);
    END_OP
}

FN(CRB_OP_FLAGS_FALSE)
{
    USES_FLAGS(CRB_FLAG_LOW_STACK);
    if (self->stack.length < 1)
    {
        set_result(self, false);
        return;
    }
}

FN(CRB_OP_FLAGS_INVERT)
{
    if (self->stack.length < 1)
    {
        set_result(self, false);
        return;
    }
}

FN(CRB_OP_FLAGS_PUSH)
{
    if (self->stack.length == 0)
    {
        set_result(self, false);
        return;
    }
}

FN(CRB_OP_FLAGS_TEST)
{
    
}

FN(CRB_OP_OPERATION_ADD)
{
    
}

FN(CRB_OP_OPERATION_SUB)
{
    
}

FN(CRB_OP_OPERATION_MUL)
{
    
}

FN(CRB_OP_OPERATION_DIV)
{
    
}

FN(CRB_OP_OPERATION_POW)
{
    
}

FN(CRB_OP_OPERATION_ROOT)
{
    
}

FN(CRB_OP_OPERATION_MOD)
{
    
}

FN(CRB_OP_OPERATION_OR)
{
    
}

FN(CRB_OP_OPERATION_AND)
{
    
}

FN(CRB_OP_OPERATION_XOR)
{
    
}

FN(CRB_OP_OPERATION_RSHIFT)
{
    
}

FN(CRB_OP_OPERATION_LSHIFT)
{
    
}

FN(CRB_OP_OPERATION_INVERT)
{
    
}

FN(CRB_OP_STACK_PUSH)
{
    
}

FN(CRB_OP_STACK_POP)
{
    
}

FN(CRB_OP_STACK_SWAP)
{
    
}

FN(CRB_OP_STACK_SAVE)
{
    
}

FN(CRB_OP_STACK_LOAD)
{
    
}

FN(CRB_OP_STACK_ANY)
{
    
}

FN(CRB_OP_STACK_DUPE)
{
    
}

FN(CRB_OP_STACK_CLEAR)
{
    
}

FN(CRB_OP_HEAP_RSHIFT)
{
    
}

FN(CRB_OP_HEAP_LSHIFT)
{
    
}

FN(CRB_OP_HEAP_JUMP)
{
    
}

FN(CRB_OP_HEAP_HOME)
{
    
}

FN(CRB_OP_HEAP_INCREMENT)
{
    
}

FN(CRB_OP_HEAP_DECREMENT)
{
    
}

FN(CRB_OP_HEAP_NULL)
{
    
}

FN(CRB_OP_HEAP_CLEAR)
{
    
}

FN(CRB_OP_IO_WRITE)
{
    
}

FN(CRB_OP_IO_READ)
{
    
}

FN(CRB_OP_IO_ANY)
{
    
}

FN(CRB_OP_MAP_READ)
{
    
}

FN(CRB_OP_MAP_WRITE)
{
    
}

FN(CRB_OP_MAP_NULL)
{
    
}

FN(CRB_OP_MAP_JUMP)
{
    
}

FN(CRB_OP_MAP_WINCREMENT)
{
    
}

FN(CRB_OP_MAP_WDECREMENT)
{
    
}

FN(CRB_OP_MAP_HINCREMENT)
{
    
}

FN(CRB_OP_MAP_HDECREMENT)
{
    
}

FN(CRB_OP_MAP_READWIDTH)
{
    
}

FN(CRB_OP_MAP_READHEIGHT)
{
    
}

FN(CRB_OP_MAP_READX)
{
    
}

FN(CRB_OP_MAP_READY)
{
    
}

crb_error_t crb_state_init()
{
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_CONDITIONALTRAVERSAL] = FNREF(CRB_OP_SET_CONDITIONALTRAVERSAL);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_COMPARE] = FNREF(CRB_OP_SET_COMPARE);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_FLAGS] = FNREF(CRB_OP_SET_FLAGS);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_OPERATION] = FNREF(CRB_OP_SET_OPERATION);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_STACK] = FNREF(CRB_OP_SET_STACK);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_HEAP] = FNREF(CRB_OP_SET_HEAP);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_IO] = FNREF(CRB_OP_SET_IO);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_SET_MAP] = FNREF(CRB_OP_SET_MAP);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_RIGHT] = FNREF(CRB_OP_FLOW_RIGHT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_LEFT] = FNREF(CRB_OP_FLOW_LEFT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_DOWN] = FNREF(CRB_OP_FLOW_DOWN);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_UP] = FNREF(CRB_OP_FLOW_UP);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_FSREFLECT] = FNREF(CRB_OP_FLOW_FSREFLECT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_BSREFLECT] = FNREF(CRB_OP_FLOW_BSREFLECT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_VREFLECT] = FNREF(CRB_OP_FLOW_VREFLECT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_HREFLECT] = FNREF(CRB_OP_FLOW_HREFLECT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_PREFLECT] = FNREF(CRB_OP_FLOW_PREFLECT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_XREFLECT] = FNREF(CRB_OP_FLOW_XREFLECT);
    FN_TABLE[CRB_MODE_TRAVERSAL][CRB_OP_FLOW_SREFLECT] = FNREF(CRB_OP_FLOW_SREFLECT);

    FN_TABLE[CRB_MODE_COMPARISON][CRB_OP_COMPARE_GREATERTHAN] = FNREF(CRB_OP_COMPARE_GREATERTHAN);
    FN_TABLE[CRB_MODE_COMPARISON][CRB_OP_COMPARE_LESSTHAN] = FNREF(CRB_OP_COMPARE_LESSTHAN);
    FN_TABLE[CRB_MODE_COMPARISON][CRB_OP_COMPARE_EQUAL] = FNREF(CRB_OP_COMPARE_EQUAL);
    FN_TABLE[CRB_MODE_COMPARISON][CRB_OP_COMPARE_NOTEQUAL] = FNREF(CRB_OP_COMPARE_NOTEQUAL);

    FN_TABLE[CRB_MODE_FLAGS][CRB_OP_FLAGS_TRUE] = FNREF(CRB_OP_FLAGS_TRUE);
    FN_TABLE[CRB_MODE_FLAGS][CRB_OP_FLAGS_FALSE] = FNREF(CRB_OP_FLAGS_FALSE);
    FN_TABLE[CRB_MODE_FLAGS][CRB_OP_FLAGS_INVERT] = FNREF(CRB_OP_FLAGS_INVERT);
    FN_TABLE[CRB_MODE_FLAGS][CRB_OP_FLAGS_PUSH] = FNREF(CRB_OP_FLAGS_PUSH);
    FN_TABLE[CRB_MODE_FLAGS][CRB_OP_FLAGS_TEST] = FNREF(CRB_OP_FLAGS_TEST);

    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_ADD] = FNREF(CRB_OP_OPERATION_ADD);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_SUB] = FNREF(CRB_OP_OPERATION_SUB);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_MUL] = FNREF(CRB_OP_OPERATION_MUL);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_DIV] = FNREF(CRB_OP_OPERATION_DIV);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_POW] = FNREF(CRB_OP_OPERATION_POW);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_ROOT] = FNREF(CRB_OP_OPERATION_ROOT);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_MOD] = FNREF(CRB_OP_OPERATION_MOD);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_OR] = FNREF(CRB_OP_OPERATION_OR);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_AND] = FNREF(CRB_OP_OPERATION_AND);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_XOR] = FNREF(CRB_OP_OPERATION_XOR);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_RSHIFT] = FNREF(CRB_OP_OPERATION_RSHIFT);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_LSHIFT] = FNREF(CRB_OP_OPERATION_LSHIFT);
    FN_TABLE[CRB_MODE_OPERATION][CRB_OP_OPERATION_INVERT] = FNREF(CRB_OP_OPERATION_INVERT);

    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_PUSH] = FNREF(CRB_OP_STACK_PUSH);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_POP] = FNREF(CRB_OP_STACK_POP);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_SWAP] = FNREF(CRB_OP_STACK_SWAP);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_SAVE] = FNREF(CRB_OP_STACK_SAVE);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_LOAD] = FNREF(CRB_OP_STACK_LOAD);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_ANY] = FNREF(CRB_OP_STACK_ANY);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_DUPE] = FNREF(CRB_OP_STACK_DUPE);
    FN_TABLE[CRB_MODE_STACK][CRB_OP_STACK_CLEAR] = FNREF(CRB_OP_STACK_CLEAR);

    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_RSHIFT] = FNREF(CRB_OP_HEAP_RSHIFT);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_LSHIFT] = FNREF(CRB_OP_HEAP_LSHIFT);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_JUMP] = FNREF(CRB_OP_HEAP_JUMP);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_HOME] = FNREF(CRB_OP_HEAP_HOME);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_INCREMENT] = FNREF(CRB_OP_HEAP_INCREMENT);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_DECREMENT] = FNREF(CRB_OP_HEAP_DECREMENT);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_NULL] = FNREF(CRB_OP_HEAP_NULL);
    FN_TABLE[CRB_MODE_HEAP][CRB_OP_HEAP_CLEAR] = FNREF(CRB_OP_HEAP_CLEAR);

    FN_TABLE[CRB_MODE_IO][CRB_OP_IO_WRITE] = FNREF(CRB_OP_IO_WRITE);
    FN_TABLE[CRB_MODE_IO][CRB_OP_IO_READ] = FNREF(CRB_OP_IO_READ);
    FN_TABLE[CRB_MODE_IO][CRB_OP_IO_ANY] = FNREF(CRB_OP_IO_ANY);
    FN_TABLE[CRB_MODE_IO][CRB_OP_MAP_READ] = FNREF(CRB_OP_MAP_READ);

    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_WRITE] = FNREF(CRB_OP_MAP_WRITE);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_NULL] = FNREF(CRB_OP_MAP_NULL);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_JUMP] = FNREF(CRB_OP_MAP_JUMP);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_WINCREMENT] = FNREF(CRB_OP_MAP_WINCREMENT);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_WDECREMENT] = FNREF(CRB_OP_MAP_WDECREMENT);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_HINCREMENT] = FNREF(CRB_OP_MAP_HINCREMENT);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_HDECREMENT] = FNREF(CRB_OP_MAP_HDECREMENT);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_READWIDTH] = FNREF(CRB_OP_MAP_READWIDTH);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_READHEIGHT] = FNREF(CRB_OP_MAP_READHEIGHT);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_READX] = FNREF(CRB_OP_MAP_READX);
    FN_TABLE[CRB_MODE_PLAYGROUND][CRB_OP_MAP_READY] = FNREF(CRB_OP_MAP_READY);

    return crb_error_ok;
}

crb_error_t crb_state_step(crb_state_t* self)
{
    return crb_error_ok;
}