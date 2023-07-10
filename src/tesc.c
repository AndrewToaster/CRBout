#include "tesc.h"
#include <stdio.h>
#include <stdlib.h>

#if __unix__
#include <stdio.h>
#include <termios.h>
static struct termios t_old, t_current;
static bool getch_started = false;
#endif
#if _WIN32
#include <conio.h>
#endif

char crb_tesc_getch() {
#if __unix__
  if (!getch_started)
    crb_tesc_getchStart();
  char ch = getchar();
  if (!getch_started)
    crb_tesc_getchStop();
  return ch;
#endif
#if _WIN32
  return getch();
#endif
}

void crb_tesc_getchStart() {
#if __unix__
  getch_started = true;
  tcgetattr(0, &t_old);         /* grab old terminal i/o settings */
  t_current = t_old;            /* make new settings same as old settings */
  t_current.c_lflag &= ~ICANON; /* disable buffered i/o */
  t_current.c_lflag &= ~ECHO;   /* set no echo mode */
  tcsetattr(0, TCSANOW,
            &t_current); /* use these new terminal i/o settings now */
#endif
}

void crb_tesc_getchStop() {
#if __unix__
  getch_started = false;
  tcsetattr(0, TCSANOW, &t_old);
#endif
}