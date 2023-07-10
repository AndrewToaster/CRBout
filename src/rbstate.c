#include "rbstate.h"
#include "rbio.h"
#include <stdbool.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <limits.h>


#define _(x) (((char)(x)) - 33)
#define FN_REF(x) crb_op_ ## x
#define FN_OP(x) static bool FN_REF(x) (crb_state_t* self)
#define FN_T(mode, op) FN_TABLE[mode][op] = FN_REF(op)
#define ERR_JUMP() CRB_ERR__ESETDIAG; longjmp(_jump_data, 1)
#define RESET_FLAG(flag) self->flags &= ~flag
#define SET_FLAG(flag) self->flags |= flag
#define STACK_LEAST(x) { RESET_FLAG(CRB_FLAGS_STACK_LOW); if (self->stack.top < x) { SET_FLAG(CRB_FLAGS_STACK_LOW); return false; } }
#define STACK_POP(id) crb_value_t (id); { if (CRB_ERR__EISFAIL(crb_stack_pop(&(self->stack), &(id)))) { ERR_JUMP(); } }
#define STACK_PUSH(value) { if (CRB_ERR__EISFAIL(crb_stack_push(&(self->stack), value))) { ERR_JUMP(); } }
#define CHECK_RESULT(expr) { RESET_FLAG(CRB_FLAGS_RESULT); if (expr) { SET_FLAG(CRB_FLAGS_RESULT); } }
#define _ASSERT_OK(expr) { if (CRB_ERR__EISFAIL(expr)) { ERR_JUMP(); } }
#define CONDTRAVERSAL_GUARD() { if (self->mode == CRB_MODE_CONDTRAVERSE && !(self->flags & CRB_FLAGS_RESULT)) return true; }

typedef bool (*crb_op_fn)(crb_state_t* self);
static jmp_buf _jump_data;
static void crb_state_advance(const crb_state_t *self, crb_pos_t *pos);

typedef enum crb_op {
    // Two special kids
    OP_HALT = _('~'),
    OP_RESET = _(';'),

    OP_SET_CONDTRAVERSE = _('@'),
    OP_SET_COMPARE = _('?'),
    OP_SET_FLAGS = _('&'),
    OP_SET_OPERATION = _('%'),
    OP_SET_STACK = _('='),
    OP_SET_HEAP = _('['),
    OP_SET_IO = _('$'),
    OP_SET_PG = _('#'),
    OP_FLOW_RIGHT = _('>'),
    OP_FLOW_LEFT = _('<'),
    OP_FLOW_DOWN = _('v'),
    OP_FLOW_UP = _('^'),
    OP_FLOW_FSREFLECT = _('/'),
    OP_FLOW_BSREFLECT = _('\\'),
    OP_FLOW_VREFLECT = _('-'),
    OP_FLOW_HREFLECT = _('|'),
    OP_FLOW_PREFLECT = _('+'),
    OP_FLOW_XREFLECT = _('x'),
    OP_FLOW_SREFLECT = _('*'),
    OP_COMPARE_GREATERTHAN = _('>'),
    OP_COMPARE_LESSTHAN = _('<'),
    OP_COMPARE_EQUAL = _('='),
    OP_COMPARE_NOTEQUAL = _('!'),
    OP_FLAGS_SET = _('|'),
    OP_FLAGS_CLEAR = _('&'),
    OP_FLAGS_INVERT = _('^'),
    OP_FLAGS_PUSH = _('>'),
    OP_FLAGS_TEST = _('?'),
    OP_OPERATION_ADD = _('+'),
    OP_OPERATION_SUB = _('-'),
    OP_OPERATION_MUL = _('*'),
    OP_OPERATION_DIV = _('/'),
    OP_OPERATION_POW = _('^'),
    OP_OPERATION_LOG = _('L'),
    OP_OPERATION_ROOT = _('\\'),
    OP_OPERATION_MOD = _('%'),
    OP_OPERATION_OR = _('|'),
    OP_OPERATION_AND = _('&'),
    OP_OPERATION_XOR = _('v'),
    OP_OPERATION_RSHIFT = _('>'),
    OP_OPERATION_LSHIFT = _('<'),
    OP_OPERATION_INVERT = _('!'),
    OP_STACK_PUSH = _('+'),
    OP_STACK_POP = _('-'),
    OP_STACK_SWAP = _('*'),
    OP_STACK_SAVE = _('>'),
    OP_STACK_LOAD = _('<'),
    OP_STACK_ANY = _('?'),
    OP_STACK_DUPE = _(':'),
    OP_STACK_CLEAR = _('&'),
    OP_HEAP_RSHIFT = _('>'),
    OP_HEAP_LSHIFT = _('<'),
    OP_HEAP_JUMP = _('#'),
    OP_HEAP_HOME = _('*'),
    OP_HEAP_INCREMENT = _('+'),
    OP_HEAP_DECREMENT = _('-'),
    OP_HEAP_NULL = _('0'),
    OP_HEAP_CLEAR = _('&'),
    OP_IO_WRITE = _('+'),
    OP_IO_READ = _('-'),
    OP_IO_ANY = _('?'),
    OP_PG_WRITE = _('+'),
    OP_PG_READ = _('-'),
    OP_PG_NULL = _('0'),
    OP_PG_JUMP = _('#'),
    OP_PG_RESIZE = _('*'),
    OP_PG_READWIDTH = _('W'),
    OP_PG_READHEIGHT = _('H'),
    OP_PG_READX = _('X'),
    OP_PG_READY = _('Y')
} crb_op_e;


FN_OP(OP_SET_CONDTRAVERSE) {
    self->mode = CRB_MODE_CONDTRAVERSE;
    return true;
}

FN_OP(OP_SET_OPERATION) {
    self->mode = CRB_MODE_OPERATION;
    return true;
}

FN_OP(OP_SET_COMPARE) {
    self->mode = CRB_MODE_COMPARE;
    return true;
}

FN_OP(OP_SET_FLAGS) {
    self->mode = CRB_MODE_FLAGS;
    return true;
}
FN_OP(OP_SET_STACK) {
    self->mode = CRB_MODE_STACK;
    return true;
}
FN_OP(OP_SET_HEAP) {
    self->mode = CRB_MODE_HEAP;
    return true;
}
FN_OP(OP_SET_IO) {
    self->mode = CRB_MODE_IO;
    return true;
}

FN_OP(OP_SET_PG) {
    self->mode = CRB_MODE_PLAYGROUND;
    return true;
}

FN_OP(OP_FLOW_RIGHT) {
    CONDTRAVERSAL_GUARD();
    self->dir = CRB_DIR_RIGHT;
    return true;
}

FN_OP(OP_FLOW_LEFT) {
    CONDTRAVERSAL_GUARD();
    self->dir = CRB_DIR_LEFT;
    return true;
}

FN_OP(OP_FLOW_DOWN) {
    CONDTRAVERSAL_GUARD();
    self->dir = CRB_DIR_DOWN;
    return true;
}

FN_OP(OP_FLOW_UP) {
    CONDTRAVERSAL_GUARD();
    self->dir = CRB_DIR_UP;
    return true;
}

FN_OP(OP_FLOW_FSREFLECT) {
    CONDTRAVERSAL_GUARD();
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
    return true;
}

FN_OP(OP_FLOW_BSREFLECT) {
    CONDTRAVERSAL_GUARD();
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
    return true;
}

FN_OP(OP_FLOW_VREFLECT) {
    CONDTRAVERSAL_GUARD();
    if (self->dir & CRB_DIR_UPDOWN)
    {
        self->dir &= CRB_DIR_UPDOWN;
    }
    else
    {
        self->dir ^= CRB_DIR_LEFTRIGHT;
    }
    return true;
}

FN_OP(OP_FLOW_HREFLECT) {
    CONDTRAVERSAL_GUARD();
    if (self->dir & CRB_DIR_LEFTRIGHT)
    {
        self->dir &= CRB_DIR_LEFTRIGHT;
    }
    else
    {
        self->dir ^= CRB_DIR_UPDOWN;
    }
    return true;
}

FN_OP(OP_FLOW_PREFLECT) {
    CONDTRAVERSAL_GUARD();
    self->dir = 1 >> (rand() % 4);
    return true;
}

FN_OP(OP_FLOW_XREFLECT) {
    CONDTRAVERSAL_GUARD();
    self->dir = 1 >> (rand() % 2) | 1 >> (2 + (rand() % 2));
    return true;
}

FN_OP(OP_FLOW_SREFLECT) {
    CONDTRAVERSAL_GUARD();
    self->dir = rand() % (1 << 4);
    return true;
}

FN_OP(OP_COMPARE_GREATERTHAN) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    CHECK_RESULT(a > b);
    return true;
}

FN_OP(OP_COMPARE_LESSTHAN) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    CHECK_RESULT(a < b);
    return true;
}

FN_OP(OP_COMPARE_EQUAL) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    CHECK_RESULT(a == b);
    return true;
}

FN_OP(OP_COMPARE_NOTEQUAL) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    CHECK_RESULT(a != b);
    return true;
}

FN_OP(OP_FLAGS_SET) {
    STACK_LEAST(1);
    STACK_POP(a);
    self->flags |= a;
    return true;
}

FN_OP(OP_FLAGS_CLEAR) {
    STACK_LEAST(1);
    STACK_POP(a);
    self->flags &= ~a;
    return true;
}

FN_OP(OP_FLAGS_INVERT) {
    STACK_LEAST(1);
    STACK_POP(a);
    self->flags ^= a;
    return true;
}

FN_OP(OP_FLAGS_PUSH) {
    STACK_PUSH(self->flags);
    return true;
}

FN_OP(OP_FLAGS_TEST) {
    STACK_LEAST(1);
    STACK_POP(a);
    CHECK_RESULT((self->flags & a) == a);
    return true;
}

FN_OP(OP_OPERATION_ADD) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(a + b);
    return true;
}

FN_OP(OP_OPERATION_SUB) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(a - b);
    return true;
}

FN_OP(OP_OPERATION_MUL) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(a * b);
    return true;
}

FN_OP(OP_OPERATION_DIV) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    
    RESET_FLAG(CRB_FLAGS_DIVISION_BY_ZERO);
    if (b == 0) { SET_FLAG(CRB_FLAGS_DIVISION_BY_ZERO); return false; }

    STACK_PUSH(a / b);
    return true;
}

FN_OP(OP_OPERATION_POW) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    
    RESET_FLAG(CRB_FLAGS_TRUNCATED);
    RESET_FLAG(CRB_FLAGS_OVER_UNDER_FLOW);
    crb_value_t c;
    if (b < 0)
    {
        c = 0;
        SET_FLAG(CRB_FLAGS_TRUNCATED);
    }
    else if (b == 0)
    {
        c = 1;
    }
    else
    {
        bool overflow = false;
        crb_value_t d = c = a;
        for (size_t i = 0; i < b - 1; i++)
        {
            if (!c)
            {
                c = 0;
                break;
            }
            c *= a;
            if (!overflow && c < d) 
            { 
                overflow = true; 
                SET_FLAG(CRB_FLAGS_OVER_UNDER_FLOW); 
            }
            d = c;
        }
    }
    STACK_PUSH(c);

    return true;
}

FN_OP(OP_OPERATION_LOG) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    
    RESET_FLAG(CRB_FLAGS_TRUNCATED);
    RESET_FLAG(CRB_FLAGS_INVALID_VALUE);
    crb_value_t c;
    if (a == 0 || b <= 0)
    {
        SET_FLAG(CRB_FLAGS_INVALID_VALUE);
        return false;
    }
    else if (a == 1)
    {
        c = 1;
    }
    else
    {   
        //? Alternative
        //! No
        //for (; a >= b; c++) a /= b;
        double result = log(a) / log(b);
        if (fabs(round(result) - result) > 1.19209290e-7) SET_FLAG(CRB_FLAGS_TRUNCATED);
        c = result;
    }
    STACK_PUSH(c);

    return true;
}

FN_OP(OP_OPERATION_ROOT) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    
    RESET_FLAG(CRB_FLAGS_TRUNCATED);
    RESET_FLAG(CRB_FLAGS_INVALID_VALUE);
    if (b == 0)
    {
        SET_FLAG(CRB_FLAGS_INVALID_VALUE);
        return false;
    }
    double result = round(pow(a, 1./b));
    if (fabs(round(result) - result) > 1.19209290e-7) SET_FLAG(CRB_FLAGS_TRUNCATED);
    STACK_PUSH(result);
    return true;
}

FN_OP(OP_OPERATION_MOD) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);

    RESET_FLAG(CRB_FLAGS_INVALID_VALUE);
    if (b == 0)
    {
        SET_FLAG(CRB_FLAGS_INVALID_VALUE);
        return false;
    }

    STACK_PUSH(a % b);
    return true;
}

FN_OP(OP_OPERATION_OR) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(a | b);
    return true;
}

FN_OP(OP_OPERATION_AND) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(a & b);
    return true;
}

FN_OP(OP_OPERATION_XOR) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(a ^ b);
    return true;
}

FN_OP(OP_OPERATION_LSHIFT) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);

    RESET_FLAG(CRB_FLAGS_INVALID_VALUE);
    if (b < 0)
    {
        SET_FLAG(CRB_FLAGS_INVALID_VALUE);
        return false;
    }

    STACK_PUSH(a << b);
    return true;
}

FN_OP(OP_OPERATION_RSHIFT) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);

    RESET_FLAG(CRB_FLAGS_INVALID_VALUE);
    if (b < 0)
    {
        SET_FLAG(CRB_FLAGS_INVALID_VALUE);
        return false;
    }

    STACK_PUSH(a >> b);
    return true;
}

FN_OP(OP_OPERATION_INVERT) {
    STACK_LEAST(1);
    STACK_POP(a);
    STACK_PUSH(~a);
    return true;
}

FN_OP(OP_STACK_PUSH) {
    int ctr = 0;
    char digits[22] = { 0 };
    char32_t symbol;
    crb_pos_t pos = self->pos;

    crb_state_advance(self, &pos);
    _ASSERT_OK(crb_playground_get(&(self->pg), pos.x, pos.y, &symbol));
    if (symbol == '-')
    {
        digits[0] = '-';
        ctr++;
    }

    while (symbol == '0')
    {
        crb_state_advance(self, &pos);
        _ASSERT_OK(crb_playground_get(&(self->pg), pos.x, pos.y, &symbol));
    }
    
    while (ctr < 21)
    {
        if (symbol < '0' || symbol > '9')
        {
            break;
        }
        digits[ctr++] = symbol;
        self->pos = pos;
        crb_state_advance(self, &pos);
        _ASSERT_OK(crb_playground_get(&(self->pg), pos.x, pos.y, &symbol));
    }
    
    STACK_PUSH(atoll(digits));

    return true;
}

FN_OP(OP_STACK_POP) {
    STACK_LEAST(1);
    STACK_POP(_);
    return true;
}

FN_OP(OP_STACK_SWAP) {
    STACK_LEAST(2);
    STACK_POP(b);
    STACK_POP(a);
    STACK_PUSH(b);
    STACK_PUSH(a);
    return true;
}

FN_OP(OP_STACK_SAVE) {
    STACK_LEAST(1);
    STACK_POP(a);
    _ASSERT_OK(crb_heap_set(&(self->heap), self->pointer, a));
    return true;
}

FN_OP(OP_STACK_LOAD) {
    crb_value_t val;
    _ASSERT_OK(crb_heap_get(&(self->heap), self->pointer, &val));
    STACK_PUSH(val);
    return true;
}

FN_OP(OP_STACK_ANY) {
    CHECK_RESULT(self->stack.top != 0);
    return true;
}

FN_OP(OP_STACK_DUPE) {
    STACK_LEAST(1);
    STACK_POP(a);
    STACK_PUSH(a);
    STACK_PUSH(a);
    return true;
}

FN_OP(OP_STACK_CLEAR) {
    while (self->stack.top)
    {
        STACK_POP(_);
    }
    return true;
}

FN_OP(OP_HEAP_LSHIFT) {
    self->pointer--;
    return true;
}

FN_OP(OP_HEAP_RSHIFT) {
    self->pointer++;
    return true;
}

FN_OP(OP_HEAP_JUMP) {
    STACK_LEAST(1);
    STACK_POP(a);
    self->pointer = a;
    return true;
}

FN_OP(OP_HEAP_HOME) {
    self->pointer = 0;
    return true;
}

FN_OP(OP_HEAP_INCREMENT) {
    crb_value_t val;
    _ASSERT_OK(crb_heap_get(&(self->heap), self->pointer, &val));
    _ASSERT_OK(crb_heap_set(&(self->heap), self->pointer, val + 1));
    return true;
}

FN_OP(OP_HEAP_DECREMENT) {
    crb_value_t val;
    _ASSERT_OK(crb_heap_get(&(self->heap), self->pointer, &val));
    _ASSERT_OK(crb_heap_set(&(self->heap), self->pointer, val - 1));
    return true;
}

FN_OP(OP_HEAP_NULL) {
    _ASSERT_OK(crb_heap_set(&(self->heap), self->pointer, 0));
    return true;
}

FN_OP(OP_HEAP_CLEAR) {
    _ASSERT_OK(crb_heap_clear(&(self->heap)));
    return true;
}

FN_OP(OP_IO_ANY) {
    bool result;
    _ASSERT_OK(crb_io_any(self->in, &result));
    RESET_FLAG(CRB_FLAGS_RESULT);
    if (result)
    {
        SET_FLAG(CRB_FLAGS_RESULT);
    }
    return true;
}

FN_OP(OP_IO_READ) {
    int c;
    _ASSERT_OK(crb_io_readChar(self->in, &c));
    STACK_PUSH(c);
    return true;
}

FN_OP(OP_IO_WRITE) {
    STACK_LEAST(1);
    STACK_POP(c);

    RESET_FLAG(CRB_FLAGS_IO_ERROR);
    CRB_ERR_TRY(crb_io_writeChar(self->out, c), except, {
        if (except.code == CRB_ERR_PARSE_ERROR)
        {
            SET_FLAG(CRB_FLAGS_IO_ERROR);
        }
        else
        {
            ERR_JUMP();
        }
    });
    return true;
}

FN_OP(OP_PG_WRITE) {
    STACK_LEAST(3);
    STACK_POP(val);
    STACK_POP(y);
    STACK_POP(x);
    RESET_FLAG(CRB_FLAGS_INVALID_VALUE);
    if (val > CRB_CHAR_MAX || val < CRB_CHAR_MIN)
    {
        SET_FLAG(CRB_FLAGS_INVALID_VALUE);
        return false;
    }
    _ASSERT_OK(crb_playground_set(&(self->pg), x % self->pg.width, y % self->pg.height, (char32_t)val));
    return true;
}

FN_OP(OP_PG_READ) {
    STACK_LEAST(2);
    STACK_POP(y);
    STACK_POP(x);
    char32_t val;
    _ASSERT_OK(crb_playground_get(&(self->pg), x % self->pg.width, y % self->pg.height, &val));
    STACK_PUSH(val);
    return true;
}

FN_OP(OP_PG_NULL) {
    STACK_LEAST(2);
    STACK_POP(y);
    STACK_POP(x);
    _ASSERT_OK(crb_playground_set(&(self->pg), x % self->pg.width, y % self->pg.height, '\0'));
    return true;
}

FN_OP(OP_PG_JUMP) {
    STACK_LEAST(2);
    STACK_POP(y);
    STACK_POP(x);
    self->pos.x = x % self->pg.width;
    self->pos.y = y % self->pg.height;
    return true;
}

FN_OP(OP_PG_RESIZE) {
    STACK_LEAST(2);
    STACK_POP(h);
    STACK_POP(w);
    _ASSERT_OK(crb_playground_resize(&(self->pg), w, h));
    return true;
}

FN_OP(OP_PG_READWIDTH) {
    STACK_PUSH(self->pg.width);
    return true;
}

FN_OP(OP_PG_READHEIGHT) {
    STACK_PUSH(self->pg.height);
    return true;
}

FN_OP(OP_PG_READX) {
    STACK_PUSH(self->pos.x);
    return true;
}

FN_OP(OP_PG_READY) {
    STACK_PUSH(self->pos.y);
    return true;
}


static crb_op_fn FN_TABLE[9][_('~') - 1];
static bool initialized = false;

__attribute__((hot))
static void crb_state_advance(const crb_state_t *self, crb_pos_t *pos)
{
    if ((self->dir & CRB_DIR_LEFT) != 0)
    {
        if (self->pos.x)
        {
            pos->x = self->pos.x - 1;
        }
        else
        {
            pos->x = self->pg.width - 1;
        }
    }
    else if ((self->dir & CRB_DIR_RIGHT) != 0)
    {
        pos->x = (self->pos.x + 1) % self->pg.width;
    }

    if ((self->dir & CRB_DIR_UP) != 0)
    {
        if (self->pos.y)
        {
            pos->y = self->pos.y - 1;
        }
        else
        {
            pos->y = self->pg.height - 1;
        }
    }
    else if ((self->dir & CRB_DIR_DOWN) != 0)
    {
        pos->y = (self->pos.y + 1) % self->pg.height;
    }
}

crb_error_t crb_state_init()
{
    FN_T(CRB_MODE_TRAVERSE, OP_SET_CONDTRAVERSE);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_OPERATION);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_COMPARE);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_FLAGS);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_STACK);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_HEAP);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_IO);
    FN_T(CRB_MODE_TRAVERSE, OP_SET_PG);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_CONDTRAVERSE);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_OPERATION);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_COMPARE);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_FLAGS);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_STACK);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_HEAP);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_IO);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_SET_PG);

    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_RIGHT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_LEFT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_DOWN);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_UP);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_FSREFLECT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_BSREFLECT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_VREFLECT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_HREFLECT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_PREFLECT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_XREFLECT);
    FN_T(CRB_MODE_TRAVERSE, OP_FLOW_SREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_RIGHT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_LEFT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_DOWN);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_UP);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_FSREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_BSREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_VREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_HREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_PREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_XREFLECT);
    FN_T(CRB_MODE_CONDTRAVERSE, OP_FLOW_SREFLECT);

    FN_T(CRB_MODE_COMPARE, OP_COMPARE_GREATERTHAN);
    FN_T(CRB_MODE_COMPARE, OP_COMPARE_LESSTHAN);
    FN_T(CRB_MODE_COMPARE, OP_COMPARE_EQUAL);
    FN_T(CRB_MODE_COMPARE, OP_COMPARE_NOTEQUAL);

    FN_T(CRB_MODE_FLAGS, OP_FLAGS_SET);
    FN_T(CRB_MODE_FLAGS, OP_FLAGS_CLEAR);
    FN_T(CRB_MODE_FLAGS, OP_FLAGS_INVERT);
    FN_T(CRB_MODE_FLAGS, OP_FLAGS_PUSH);
    FN_T(CRB_MODE_FLAGS, OP_FLAGS_TEST);

    FN_T(CRB_MODE_OPERATION, OP_OPERATION_ADD);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_SUB);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_MUL);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_DIV);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_POW);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_LOG);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_ROOT);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_MOD);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_OR);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_AND);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_XOR);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_LSHIFT);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_RSHIFT);
    FN_T(CRB_MODE_OPERATION, OP_OPERATION_INVERT);
    
    FN_T(CRB_MODE_STACK, OP_STACK_PUSH);
    FN_T(CRB_MODE_STACK, OP_STACK_POP);
    FN_T(CRB_MODE_STACK, OP_STACK_SAVE);
    FN_T(CRB_MODE_STACK, OP_STACK_LOAD);
    FN_T(CRB_MODE_STACK, OP_STACK_ANY);
    FN_T(CRB_MODE_STACK, OP_STACK_SWAP);
    FN_T(CRB_MODE_STACK, OP_STACK_DUPE);
    FN_T(CRB_MODE_STACK, OP_STACK_CLEAR);
    
    FN_T(CRB_MODE_HEAP, OP_HEAP_LSHIFT);
    FN_T(CRB_MODE_HEAP, OP_HEAP_RSHIFT);
    FN_T(CRB_MODE_HEAP, OP_HEAP_JUMP);
    FN_T(CRB_MODE_HEAP, OP_HEAP_HOME);
    FN_T(CRB_MODE_HEAP, OP_HEAP_INCREMENT);
    FN_T(CRB_MODE_HEAP, OP_HEAP_DECREMENT);
    FN_T(CRB_MODE_HEAP, OP_HEAP_NULL);
    FN_T(CRB_MODE_HEAP, OP_HEAP_CLEAR);

    FN_T(CRB_MODE_IO, OP_IO_WRITE);
    FN_T(CRB_MODE_IO, OP_IO_READ);
    FN_T(CRB_MODE_IO, OP_IO_ANY);

    FN_T(CRB_MODE_PLAYGROUND, OP_PG_WRITE);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_READ);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_NULL);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_JUMP);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_RESIZE);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_READWIDTH);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_READHEIGHT);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_READX);
    FN_T(CRB_MODE_PLAYGROUND, OP_PG_READY);

    initialized = true;
    return crb_error_ok;
}

__attribute__((hot)) crb_error_t crb_state_step(crb_state_t *self)
{
    if (!initialized) CRB_ERR_THROW(CRB_ERR_NEW(CRB_ERR_INVALID_STATE, "Call crb_state_init before stepping"));
    if (!self) CRB_ERR_THROW(crb_error_invalidPtr);
    if (self->stopped) return crb_error_ok;

    char32_t symbol;
    crb_error_t err;
    if (crb_error_isFail(err = crb_playground_get(&(self->pg), self->pos.x, self->pos.y, &symbol))) return err;

    // Check if symbol is within valid range, then execute op and check return
    if (!(symbol < 33 || symbol > 125))
    {
        crb_op_fn op = FN_TABLE[self->mode][_(symbol)];
        if (setjmp(_jump_data) != 0)
        {
            return CRB_ERR_LAST_ERROR;
        }
        
        if (op)
        {
            if (op(self))
            {
                SET_FLAG(CRB_FLAGS_SUCCESS);
            }
            else
            {
                RESET_FLAG(CRB_FLAGS_SUCCESS);
            }
        }
        else if (symbol == ';')
        {
            self->mode = CRB_MODE_TRAVERSE;
        }
    }
    else if (symbol == '~')
    {
        self->stopped = true;
        return crb_error_ok;
    }

    //?  Possibly could be implemented as such
    //!! No it can't, modulo N of the underflow is not N - 1
    //// self->pos.x = (self->pos.x + !!(self->dir & CRB_DIR_RIGHT) - !!(self->dir & CRB_DIR_LEFT)) % self->pg.width;
    //// self->pos.y = (self->pos.y + !!(self->dir & CRB_DIR_DOWN) - !!(self->dir & CRB_DIR_UP)) % self->pg.height;
    crb_state_advance(self, &(self->pos));

    return crb_error_ok;
}