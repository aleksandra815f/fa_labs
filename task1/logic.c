#include "logic.h"

#include <limits.h>
#include <stddef.h>

Status find_multiples(long long x, long long *result, int *count){//-h кратные от 1 до 100
    if (result == NULL || count == NULL) {
        return STATUS_NULL;
    }
    if (x <= 0) return STATUS_NEGATIVE;
    *count = 0;
    if (x == 0) {
        return STATUS_OK;
    }

    for (long long n = 1; n <= 100; n++) {
        if (n % x == 0) {
            result[*count] = n;
            (*count)++;
        }
    }
    return STATUS_OK;
}

Status check_prime(long long x, NumberKind *kind){//-p простое или составное
    if (kind == NULL) {
        return STATUS_NULL;
    }
    if (x <= 1) {
        *kind = NUMBER_NEITHER;
        return STATUS_OK;
    }
    if (x <= 3) {
        *kind = NUMBER_PRIME;
        return STATUS_OK;
    }
    if (x % 2 == 0) {
        *kind = NUMBER_COMPOSITE;
        return STATUS_OK;
    }
    for (long long i = 3; i <= x / i; i += 2) {
        if (x % i == 0) {
            *kind = NUMBER_COMPOSITE;
            return STATUS_OK;
        }
    }
    *kind = NUMBER_PRIME;
    return STATUS_OK;
}
Status get_hex_digits(long long x, int *digits, int *count){//-s цифры чсла в 16сс
    if (digits == NULL || count == NULL) {
        return STATUS_NULL;
    }
    if (x < 0) {
        return STATUS_NEGATIVE;
    }

    if (x == 0) {
        digits[0] = 0;
        *count = 1;
        return STATUS_OK;
    }
    int reversed[MAX_HEX_DIGITS];
    int n = 0;
    while (x > 0) {
        reversed[n] = (int)(x % 16);
        n++;
        x /= 16;
    }
    // записываем числа от старшего к младшему
    for (int i = 0; i < n; i++) {
        digits[i] = reversed[n - 1 - i];
    }
    *count = n;
    return STATUS_OK;
}

Status make_power_table(long long x, long long *table, int *cols){//-e табл степеней
    if (table == NULL || cols == NULL) {
        return STATUS_NULL;
    }
    if (x < 1 || x > TABLE_SIZE) {
        return STATUS_OUT_OF_RANGE;
    }
    for (int base = 1; base <= TABLE_SIZE; base++) {
        long long value = 1;
        for (int exp = 1; exp <= x; exp++) {
            value *= base;// максимум 10^10, в long long помещается
            table[(base - 1) * TABLE_SIZE + (exp - 1)] = value;
        }
    }
    *cols = (int)x;
    return STATUS_OK;
}

Status sum_to_x(long long x, long long *sum){//-а сумма от 1 до х
    if (sum == NULL) {
        return STATUS_NULL;
    }
    if (x < 0) {
        return STATUS_NEGATIVE;
    }
    if (x == 0) {
        *sum = 0;
        return STATUS_OK;
    }
    if (x == LLONG_MAX) {
        return STATUS_OVERFLOW;
    }

//сумма = x * (x + 1) / 2
//Одно из чисел x и x + 1 чётное, делим сразу на 2, чтобы не бфло переполнения в произведении
    long long a = x;
    long long b = x + 1;
    if (a % 2 == 0) {
        a /= 2;
    } else {
        b /= 2;
    }
    if (b > LLONG_MAX / a) { // a * b не поместятся 
        return STATUS_OVERFLOW;
    }
    *sum = a * b;
    return STATUS_OK;
}

Status factorial(long long x, long long *result)
{
    if (result == NULL) {
        return STATUS_NULL;
    }
    if (x < 0) {
        return STATUS_NEGATIVE;
    }

    long long value = 1;
    for (long long i = 2; i <= x; i++) {
        if (value > LLONG_MAX / i) {   // value * i не поместится 
            return STATUS_OVERFLOW;
        }
        value *= i;
    }
    *result = value;
    return STATUS_OK;
}