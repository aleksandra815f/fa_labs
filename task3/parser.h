#ifndef PARSER_H
#define PARSER_H

#include "status.h"

typedef enum {
    FLAG_Q,   /* -q квадратное уравнение */
    FLAG_M,   /* -m кратность чисел */
    FLAG_T    /* -t прямоугольный треугольник */
} Flag;

Status parse_flag(const char *text, Flag *flag);

Status params_count(const Flag flag, int *count);
Status parse_double(const char *text, double *number);
Status parse_integer(const char *text, long long *number);

#endif