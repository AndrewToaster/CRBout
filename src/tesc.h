/**
 * @file tesc.h
 * @author AndrewToasterr
 * @brief This is a gross violation of any and all laws of anything
 * @version 0.1
 * @date 2023-06-29
 * 
 * @copyright See LICENSE
 */

#ifndef CRBOUT_TESC_H
#define CRBOUT_TESC_H

#include <stdbool.h>
#include <stdint.h>

// -->> THE  CSI <<--
#define TESC_CSI "\e["

// Cursor control
#define TESC_MOVE_HOME TESC_CSI "1;1H"

#define TESC_MOVE_XY(x, y) TESC_CSI #y ";" #x "H"

#define ffTESC_MOVE_YX(specifier) TESC_CSI #specifier ";" #specifier "H"
#define fTESC_MOVE_YX ffTESC_MOVE_YX(%i)

#define TESC_MOVE_UP(n) TESC_CSI #n "A"
#define TESC_MOVE_DOWN(n) TESC_CSI #n "B"
#define TESC_MOVE_LEFT(n) TESC_CSI #n "D"
#define TESC_MOVE_RIGHT(n) TESC_CSI #n "C"

#define ffTESC_MOVE_UP(specifier) TESC_CSI #specifier "A"
#define ffTESC_MOVE_DOWN(specifier) TESC_CSI #specifier "B"
#define ffTESC_MOVE_LEFT(specifier) TESC_CSI #specifier "D"
#define ffTESC_MOVE_RIGHT(specifier) TESC_CSI #specifier "C"

#define fTESC_MOVE_UP ffTESC_MOVE_UP(%i)
#define fTESC_MOVE_DOWN ffTESC_MOVE_DOWN(%i)
#define fTESC_MOVE_LEFT ffTESC_MOVE_LEFT(%i)
#define fTESC_MOVE_RIGHT ffTESC_MOVE_RIGHT(%i)

#define TESC_CURSOR_SHOW TESC_CSI "?25h"
#define TESC_CURSOR_HIDE TESC_CSI "?25l"

// SGR
#define TESC_SGR_RESET TESC_CSI "0m"
#define TESC_SGR_COLOR_FG(r, g, b) TESC_CSI "38;2;" #r ";" #g ";" #b "m"
#define TESC_SGR_COLOR_BG(r, g, b) TESC_CSI "48;2;" #r ";" #g ";" #b "m"

#define fTESC_SGR_COLOR_FG ffTESC_SGR_COLOR_FG(%hhu)
#define fTESC_SGR_COLOR_BG ffTESC_SGR_COLOR_BG(%hhu)

#define ffTESC_SGR_COLOR_FG(specifier) TESC_CSI "38;2;" #specifier ";" #specifier ";" #specifier "m"
#define ffTESC_SGR_COLOR_BG(specifier) TESC_CSI "48;2;" #specifier ";" #specifier ";" #specifier "m"

#define TESC_ERASE_LINE_RIGHT TESC_CSI "0K"
#define TESC_ERASE_LINE_LEFT TESC_CSI "1K"
#define TESC_ERASE_LINE_ALL TESC_CSI "2K"

#define TESC_ALTERNATE_BUFFER_SHOW TESC_CSI "?1049h"
#define TESC_ALTERNATE_BUFFER_HIDE TESC_CSI "?1049l"

//!! THESE ARE PLATFORM SPECIFIC, WILL NOT WORK ON POSIX
//TODO: Implement a platform agnostic approach
#define GETCH_UPARROW  72
#define GETCH_DOWNARROW 80
#define GETCH_LEFTARROW 75
#define GETCH_RIGHTARROW 77

void crb_tesc_unbuffer(void);
int crb_tesc_getch(void);
void crb_tesc_restore(void);
int crb_tesc_ungetch(int);
bool crb_tesc_kbhit(void);
bool crb_tesc_showAlternateBuffer();
bool crb_tesc_hideAlternateBuffer();

#endif