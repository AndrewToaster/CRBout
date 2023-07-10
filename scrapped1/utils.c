#include <math.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>

unsigned int crb_util_digitCount(int number)
{
    if (number == 0)
        return 1;

    return (unsigned int)floor(log10(abs(number)) + 1);
}

void crb_util_randseed()
{
    srand(time(NULL));
}

uint8_t crb_util_randbyte()
{
    return rand() & 0xff;
}