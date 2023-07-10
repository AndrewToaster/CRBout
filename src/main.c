#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rbstate.h"
#include "rbio.h"
#include "utils.h"
#include "tesc.h"
#include <uchar.h>
#include <ctype.h>
#include <locale.h>
#include <pthread.h>
#include <stdatomic.h>
#include <math.h>
#include <conio.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>

#define _USEALLBG
#define ALL_BG TESC_SGR_COLOR_BG(50, 50, 50)
#define eprintf(fmt, args...) fprintf(stderr, fmt, ##args)

/**
 * @brief Hashtable for print formats
 * First index is whether or not we have a valid character
 * Second index is whether or not we highlight
 */
static const char *format[2][2] = {
    // Invalid char
    { 
        #ifdef _USEALLBG
        ALL_BG
        #endif
        TESC_SGR_COLOR_FG(200, 0, 0) "?" TESC_SGR_RESET, // No highlight
        TESC_SGR_COLOR_BG(0, 200, 0) TESC_SGR_COLOR_FG(200, 0, 0) "?" TESC_SGR_RESET // Highlight
    },

    // Valid char
    { 
        #ifdef _USEALLBG
        ALL_BG
        #endif
        TESC_SGR_COLOR_FG(255, 255, 255) "%c" TESC_SGR_RESET, // No highlight
        TESC_SGR_COLOR_BG(0, 200, 0) TESC_SGR_COLOR_FG(255, 255, 255) "%c" TESC_SGR_RESET // Highlight
    }
};

// Don't question it
static const char char_norm_table[] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 
    92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, ' ',
};

static CRB_ERROR debug_loop(FILE *code);
static CRB_ERROR run_loop(FILE *code);

static int handle_error(crb_error_t err)
{   
    printf(TESC_SGR_COLOR_BG(100, 10, 10) "[ERROR] code: %lli\n%s\nin %s at %s:%i", err.code, err.message, CRB_ERR_LAST_ERROR_FUNC, CRB_ERR_LAST_ERROR_FILE, CRB_ERR_LAST_ERROR_LINE);
    if (err.message)
    {
        printf("\n%s", err.data);
        CRB_ERR_FREE(err);
    }
    printf("\n" TESC_SGR_RESET);
    return err.code;
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        printf("Usage: crbout <option> <file>\n" 
               "where allowed option values are\n"
               "\trun\texecutes the file\n"
               "\tdebug\texecutes the file in debug mode.\n"
               "\t\tDebug information is written to the standard output\n"
               "\t\tProgram output is written to the standard error\n"
               "where file is a valid path to a file containing the program");
        return 0;
    }
    else if (argc < 3)
    {
        printf("Missing required positional argument at position 2. Allowed values are:\n>[Valid file path]");
        return 1;
    }

    struct stat data;
    if (stat(argv[2], &data))
    {
        printf("File '%s' does not exist", argv[2]);
        return 2;
    }

    FILE* code = fopen(argv[2], "r");

    if (strcasecmp(argv[1], "run") == 0)
    {
        CRB_ERR_ASSERT_OKX(run_loop(code), { return handle_error(CRB_ERR_LAST_ERROR); });
    }
    else if (strcasecmp(argv[1], "debug") == 0)
    {
        CRB_ERR_ASSERT_OKX(debug_loop(code), { return handle_error(CRB_ERR_LAST_ERROR); });
    }
    else
    {
        printf("Incorrect required positional argument at position 1. Allowed values are:\n>run\n>debug");
        return 3;
    }

    return 0;
}

static crb_error_t run_loop(FILE *code)
{
    CRB_ERR_ASSERT_OK(crb_state_init());

    crb_playground_t pg;
    crb_stack_t stack;
    crb_heap_t heap;

    CRB_ERR_ASSERT_OK(crb_playground_load(&pg, code));
    CRB_ERR_ASSERT_OK(crb_stack_make(&stack));
    CRB_ERR_ASSERT_OK(crb_heap_make(&heap, CRB_HEAP_BUCKET_SIZE));
    fclose(code);

    crb_state_t state = {
        .dir = CRB_DIR_RIGHT,
        .flags = 0,
        .pos = {
            .x = 0,
            .y = 0
        },
        .heap = heap,
        .mode = CRB_MODE_TRAVERSE,
        .stack = stack,
        .stopped = false,
        .pg = pg,
        .in = stdin,
        .out = stdout
    };

    while (!state.stopped)
    {
        CRB_ERR_ASSERT_OK(crb_state_step(&state));
    }

    return crb_error_ok;
}

static crb_error_t debug_loop(FILE *code)
{
    CRB_ERR_ASSERT_OK(crb_state_init());
    CRB_ERR_ASSERT_OK(crb_io_initTerm());

    crb_playground_t pg;
    crb_stack_t stack;
    crb_heap_t heap;

    CRB_ERR_ASSERT_OK(crb_playground_load(&pg, code));
    CRB_ERR_ASSERT_OK(crb_stack_make(&stack));
    CRB_ERR_ASSERT_OK(crb_heap_make(&heap, CRB_HEAP_BUCKET_SIZE));
    fclose(code);

    crb_state_t state = {
        .dir = CRB_DIR_RIGHT,
        .flags = 0,
        .pos = {
            .x = 0,
            .y = 0
        },
        .heap = heap,
        .mode = CRB_MODE_TRAVERSE,
        .stack = stack,
        .stopped = false,
        .pg = pg,
        .in = stdin,
        .out = stderr
    };

    printf(TESC_MOVE_HOME);
    for (size_t y = 0; y < pg.height; y++)
    {
        for (size_t x = 0; x < pg.width; x++)
        {
            char32_t letter;
            CRB_ERR_ASSERT_OK(crb_playground_get(&pg, x, y, &letter));
            printf(format[!(letter & ~0x7f)][0], char_norm_table[letter]);
        }
        printf("\n");   
    }
    
    printf(TESC_CURSOR_HIDE);
    char32_t letter, pl;
    size_t px, py;
    bool flag = false, ignoreNext = false;
    CRB_ERR_ASSERT_OK(crb_playground_get(&state.pg, state.pos.x, state.pos.y, &letter));
    while (!state.stopped)
    {
        if (flag)
        {
            if (ignoreNext)
            {
                ignoreNext = false;
                getch();
            }
            int c = getch();
            // Enter
            if (c == 13)
            {
                flag = false;
            }
            // STX (Ctrl-B)
            else if (c == 2)
            {
                ignoreNext = true;
                ungetch(getch());
            }
        }
        else if (kbhit())
        {
            int c = getch();
            // STX (Ctrl-B)
            if (c == 2)
            {
                flag = true;
                continue;
            }
            else
            {
                ungetch(c);
            }
        }
        px = state.pos.x;
        py = state.pos.y;
        pl = letter;

        CRB_ERR_ASSERT_OK(crb_state_step(&state));
        CRB_ERR_ASSERT_OK(crb_playground_get(&state.pg, state.pos.x, state.pos.y, &letter));
        
        printf(ffTESC_MOVE_YX(%llu) TESC_SGR_COLOR_BG(155, 50, 50) CRB_UTIL_FMT_BINU32 "\n" "[ ",
            state.pg.height + 1, 0ULL, CRB_UTIL_FMT_BINU32_VAL(state.flags));
        
        for (size_t i = 0; i < state.stack.top; i++)
        {
            printf("%lli", state.stack.raw[i]);
            if (i < state.stack.top - 1)
            {
                printf(", ");
            }
        }
        printf(" ]" TESC_SGR_RESET TESC_ERASE_LINE_RIGHT);
        

        printf(ffTESC_MOVE_YX(%llu), state.pos.y + 1, state.pos.x + 1);
        printf(format[!(letter & ~0x7f)][1], char_norm_table[letter]);

        printf(ffTESC_MOVE_YX(%llu), py + 1, px + 1);
        printf(format[!(pl & ~0x7f)][0], char_norm_table[pl]);
    }

    return crb_error_ok;
}