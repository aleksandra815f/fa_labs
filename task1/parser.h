#ifndef PARSER_H
#define PARSER_H
#include "status.h"
typedef enum {
    FLAG_H,
    FLAG_P,
    FLAG_S,
    FLAG_E,
    FLAG_A,
    FLAG_F
} Flag;
Status parse_flag(const char *text, Flag *flag); 
Status parse_number(const char *text, long long *number);

#endif