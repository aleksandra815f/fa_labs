#include "parser.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>

Status parse_double(const char *text, double *number){
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

    if (!isfinite(value)) {
        return STATUS_BAD_NUMBER;
    }
    *number = value;
    return STATUS_OK;
}