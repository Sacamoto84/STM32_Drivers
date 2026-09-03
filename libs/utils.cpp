#include "utils.h"
#include <string.h>

#define DEC 10

static char* ltoa(long value, char *buffer, int base) {
    char *ptr = buffer;
    char *ptr1 = buffer;
    char tmp;
    int negative = 0;
    unsigned long uvalue;

    if (value < 0 && base == 10) {
        negative = 1;
        uvalue = -value;
    } else {
        uvalue = value;
    }

    do {
        unsigned long remainder = uvalue % base;
        *ptr++ = (remainder < 10) ? '0' + remainder : 'a' + remainder - 10;
    } while (uvalue /= base);

    if (negative) *ptr++ = '-';

    *ptr = 0;

    // Reverse string
    ptr1 = buffer;
    ptr--;
    while (ptr1 < ptr) {
        tmp = *ptr1;
        *ptr1++ = *ptr;
        *ptr-- = tmp;
    }

    return buffer;
}

char* mUtoa(uint32_t value, char *buffer, bool clear) {
    buffer += 11;
    if (clear) *--buffer = 0;
    do {
        *--buffer = value % 10 + '0';
        value /= 10;
    } while (value != 0);
    return buffer;
}

char* mLtoa(int32_t value, char *buffer, bool clear) {
    bool minus = value < 0;
    if (minus) value = -value;
    buffer = mUtoa(value, buffer, clear);
    if (minus) *--buffer = '-';
    return buffer;
}

char* mFtoa(double value, int8_t decimals, char *buffer) {
    int32_t mant = (int32_t)value;
    value -= mant;
    uint32_t exp = 1;
    while (decimals--) exp *= 10;
    exp *= (float)value;
    buffer = ltoa(mant, buffer, DEC);
    uint8_t len = strlen(buffer);
    *(buffer + len++) = '.';
    ltoa(exp, buffer + len++, DEC);
    return buffer;
}


