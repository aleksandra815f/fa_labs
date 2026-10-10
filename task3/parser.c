#include "parser.h"

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

Status parse_flag(const char *text, Flag *flag)
{
    if (text == NULL || flag == NULL) {
        return STATUS_NULL;
    }

    if (strlen(text) != 2 || (text[0] != '-' && text[0] != '/')) {
        return STATUS_BAD_FLAG;
    }

    switch (text[1]) {
        case 'q': *flag = FLAG_Q; break;
        case 'm': *flag = FLAG_M; break;
        case 't': *flag = FLAG_T; break;
        default:
            return STATUS_BAD_FLAG;
    }
    return STATUS_OK;
}

Status params_count(const Flag flag, int *count)
{
    if (count == NULL) {
        return STATUS_NULL;
    }

    switch (flag) {
        case FLAG_Q: *count = 4; return STATUS_OK;
        case FLAG_M: *count = 2; return STATUS_OK;
        case FLAG_T: *count = 4; return STATUS_OK;
    }
    return STATUS_BAD_FLAG;
}

Status parse_double(const char *text, double *number)
{
    if (text == NULL || number == NULL) {
        return STATUS_NULL;
    }

    if (isspace((unsigned char)text[0]) || text[0] == '+') {
        return STATUS_BAD_NUMBER;
    }

    char *end = NULL;
    double value = strtod(text, &end);

    if (end == text || *end != '\0') {
        return STATUS_BAD_NUMBER;
    }

    if (isnan(value)) {
        return STATUS_BAD_NUMBER;
    }
    if (isinf(value)) {
        return STATUS_OVERFLOW;
    }

    *number = value;
    return STATUS_OK;
}

Status parse_integer(const char *text, long long *number)
{
    if (text == NULL || number == NULL) {
        return STATUS_NULL;
    }

    if (isspace((unsigned char)text[0]) || text[0] == '+') {
        return STATUS_BAD_NUMBER;
    }

    char *end = NULL;
    long long value = strtoll(text, &end, 10);

    if (end == text || *end != '\0') {
        return STATUS_BAD_NUMBER;
    }
    if (value == LLONG_MAX || value == LLONG_MIN) {
        return STATUS_BAD_NUMBER;
    }

    *number = value;
    return STATUS_OK;
}