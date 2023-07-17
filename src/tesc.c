#include "tesc.h"
#include <stdio.h>
#include <stdlib.h>

#if __unix__
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
static struct termios t_old, t_current;
static bool unbuffered = false;
static int pushback = -2;
#elif _WIN32
#include <conio.h>
#endif

int crb_tesc_getch() {
#if __unix__
    int ch;
    if (pushback != -2)
    {
        ch = pushback;
        pushback = -2;
        return ch;
    }
    if (!unbuffered)
        crb_tesc_unbuffer();
    ch = getchar();
    if (!unbuffered)
        crb_tesc_restore();
    return ch;
#elif _WIN32
    return getch();
#endif
}

void crb_tesc_unbuffer() {
#if __unix__
    unbuffered = true;
    tcgetattr(0, &t_old);         /* grab old terminal i/o settings */
    t_current = t_old;            /* make new settings same as old settings */
    t_current.c_lflag &= ~ICANON; /* disable buffered i/o */
    t_current.c_lflag &= ~ECHO;   /* set no echo mode */
    tcsetattr(0, TCSANOW,
    &t_current); /* use these new terminal i/o settings now */
#endif
}

void crb_tesc_restore() {
#if __unix__
    unbuffered = false;
    tcsetattr(0, TCSANOW, &t_old);
#endif
}

bool crb_tesc_kbhit() {
#if _WIN32
    return kbhit();
#elif __unix__
    if (pushback != -2)
        return true;
    if (!unbuffered)
        crb_tesc_unbuffer();
    int bytesWaiting;
    ioctl(0 , FIONREAD, &bytesWaiting);
    if (!unbuffered)
        crb_tesc_restore();
    return bytesWaiting != 0;
#endif
}

int crb_tesc_ungetch(int value) {
#if _WIN32
    return ungetch(value) == EOF;

#elif __unix__
    if (pushback != -2)
        return 1;
    pushback = value;
    return 0;
#endif
}