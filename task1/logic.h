#ifndef LOGIC_H
#define LOGIC_H

#include "status.h"

#define MAX_MULTIPLES 100   /* кратных числу в диапазоне 1..100 не больше 100 */
#define MAX_HEX_DIGITS 16   /* в long long не больше 16 шестнадцатеричных цифр */
#define TABLE_SIZE 10     

typedef enum { //-р
    NUMBER_PRIME,
    NUMBER_COMPOSITE,
    NUMBER_NEITHER
} NumberKind;

Status find_multiples(long long x, long long *result, int *count);
Status check_prime(long long x, NumberKind *kind);
Status get_hex_digits(long long x, int *digits, int *count);
Status make_power_table(long long x, long long *table, int *cols);
Status sum_to_x(long long x, long long *sum);
Status factorial(long long x, long long *result);

#endif