#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>


unsigned int crb_util_digitCount(int number);

void crb_util_randseed();
uint8_t crb_util_randbyte();

#define CRB_MAGIC_CAT "\\_<(= ^-^)="

#endif