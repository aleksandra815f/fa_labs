#include "parsing.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

Status parse_int(const char *str, int *out)
{
    if (str == NULL || out == NULL) {
        return STATUS_ERR_NULL_ARGUMENT;
    }
    if (str[0] == '\0') {
        return STATUS_ERR_NUMBER_INVALID;
    }
    if (isspace((unsigned char) str[0]) || str[0] == '+') {
        return STATUS_ERR_NUMBER_INVALID;
    }

    char *endptr = NULL;
    const long long value = strtoll(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        return STATUS_ERR_NUMBER_INVALID;
    }
    if (value < INT_MIN || value > INT_MAX) {
        return STATUS_ERR_NUMBER_OUT_OF_RANGE;
    }

    *out = (int) value;
    return STATUS_OK;
}