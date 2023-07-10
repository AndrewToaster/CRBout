#include "playground.h"
#include "tesc.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TEST 0

typedef void (*crb_op_fn)(int*);

static crb_op_fn crb_operations[1] = {};

int main(int argc, const char **argv) {
/*
    int x = 0, y = 0, c;
    printf(TESC_CURSOR_HIDE TESC_SGR_COLOR_BG(0, 255, 0) " ");

    //crb_tesc_setCursorVisible(false);
    crb_tesc_getchStart();
    while ((c = crb_tesc_getch()) != EOF && c != '\r') {
        if (c != 0)
            continue;

        if ((c = crb_tesc_getch()) == EOF || c == '\r')
            break;
        
        int tx = x, ty = y;
        if (c == GETCH_UPARROW && y > 0)
        {
            y--;
        }
        else if (c == GETCH_DOWNARROW)
        {
            y++;
        }
        else if (c == GETCH_LEFTARROW && x > 0)
        {
            x--;
        }
        else if (c == GETCH_RIGHTARROW)
        {
            x++;
        }
        else
        {
            continue;
        }

        // This is ungodly macro abuse, but like it works
        printf(fTESC_MOVE_YX TESC_SGR_COLOR_BG(0, 255, 0) " " TESC_SGR_RESET fTESC_MOVE_YX " ", y + 1, x + 1, ty + 1, tx + 1);
        //printf(fTESC_MOVE_YX TESC_SGR_COLOR_BG(0, 255, 0) " ", y + 1, x + 1);
        //printf(TESC_SGR_RESET fTESC_MOVE_YX " ", ty + 1, tx + 1);
        //crb_tesc_setCursorPos(x, y);
        //crb_tesc_changeColorBG(0, 255, 0);
        //printf(" ");
        //crb_tesc_resetGraphics();
        //crb_tesc_setCursorPos(tx, ty);
        //printf(" ");

    }
    crb_tesc_getchStop();
    //crb_tesc_resetGraphics();
*/
    //crb_tests_run();
    
    return 0;
}