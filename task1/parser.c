#include "parser.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

Status parse_flag(const char *text, Flag *flag){
    if (text == NULL ||flag == NULL) {
        return STATUS_NULL;
    }
    if (strlen(text) != 2 || (text[0] != '-' && text[0] != '/')) {
        return STATUS_BAD_FLAG;
    }
    switch (text[1]){
        case 'h': *flag = FLAG_H; break;
        case 'p': *flag = FLAG_P; break;
        case 's': *flag = FLAG_S; break;
        case 'e': *flag = FLAG_E; break;
        case 'a': *flag = FLAG_A; break;
        case 'f': *flag = FLAG_F; break;
        default: return STATUS_BAD_FLAG;
    }
    return STATUS_OK;
}

Status parse_number(const char *text, long long *number){
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

    if (value == LLONG_MAX && strcmp(text, "9223372036854775807") != 0) {
        return STATUS_TOO_BIG;
    }
    if (value == LLONG_MIN && strcmp(text, "-9223372036854775808") != 0) {
        return STATUS_TOO_BIG;
    }
    *number = value;
    return STATUS_OK;
}