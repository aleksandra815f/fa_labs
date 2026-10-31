#ifndef LOGIC_H
#define LOGIC_H
#include <stddef.h>
#include "status.h"

typedef struct {
    size_t line; //строка
    size_t col;  //номер символа в строке 
} Match;

typedef struct {
    const char *filename; 
    Status status;  
    Match *matches; 
    size_t match_count;  
} SearchReport;

Status search_in_files(SearchReport **out_reports, const char *needle, int file_count, ...);
void free_search_reports(SearchReport *reports, int file_count);

#endif