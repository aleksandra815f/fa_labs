#include "logic.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static int is_match(const char *text, const char *needle, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (text[i] != needle[i]) return 0;
    }
    return 1;
}


static void find_all(const char *text, const char *needle, SearchReport *report) {
    size_t text_len = 0;
    while (text[text_len] != '\0') text_len++;

    size_t needle_len = 0;
    while (needle[needle_len] != '\0') needle_len++;

    report->matches = NULL;
    report->match_count = 0;

    if (needle_len == 0 || text_len < needle_len) return;

    size_t max_matches = text_len - needle_len + 1;
    report->matches = (Match *)malloc(max_matches * sizeof(Match));
    if (!report->matches) {
        report->status = STATUS_ERR_ALLOC_FAILED;
        return;
    }

    size_t current_line = 1;
    size_t current_col = 1;

    for (size_t i = 0; i <= text_len - needle_len; i++) {
        if (is_match(&text[i], needle, needle_len)) {
            report->matches[report->match_count].line = current_line;
            report->matches[report->match_count].col = current_col;
            report->match_count++;
        }
        
        if (text[i] == '\n') {
            current_line++;
            current_col = 1;
        } else {
            current_col++;
        }
    }
}

Status search_in_files(SearchReport **out_reports, const char *needle, int file_count, ...) {
    if (!out_reports || !needle) return STATUS_ERR_NULL_ARGUMENT;
    if (file_count < 0) return STATUS_ERR_COUNT_NEGATIVE;

    *out_reports = (SearchReport *)malloc((size_t)file_count * sizeof(SearchReport));
    if (!*out_reports) return STATUS_ERR_ALLOC_FAILED;

    va_list args;
    va_start(args, file_count);

    for (int i = 0; i < file_count; i++) {
        const char *filename = va_arg(args, const char *);
        SearchReport *report = &(*out_reports)[i];
        
        report->filename = filename;
        report->status = STATUS_OK;
        report->matches = NULL;
        report->match_count = 0;

        if (!filename) {
            report->status = STATUS_ERR_NULL_ARGUMENT;
            continue;
        }

        FILE *file = fopen(filename, "r");
        if (!file) {
            report->status = STATUS_ERR_FILE_OPEN;
            continue;
        }

        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            report->status = STATUS_ERR_SEEK_FAILED;
            continue;
        }
        
        long size = ftell(file);
        if (size < 0) {
            fclose(file);
            report->status = STATUS_ERR_SEEK_FAILED;
            continue;
        }
        fseek(file, 0, SEEK_SET);

        if (size == 0) {
            fclose(file);
            continue;
        }

        char *buffer = (char *)malloc((size_t)size + 1);
        if (!buffer) {
            fclose(file);
            report->status = STATUS_ERR_ALLOC_FAILED;
            continue;
        }

        if (fread(buffer, 1, (size_t)size, file) != (size_t)size) {
            free(buffer);
            fclose(file);
            report->status = STATUS_ERR_FILE_READ;
            continue;
        }
        buffer[size] = '\0';
        fclose(file);

        find_all(buffer, needle, report);
        free(buffer);
    }

    va_end(args);
    return STATUS_OK;
}

void free_search_reports(SearchReport *reports, int file_count) {
    if (!reports) return;
    for (int i = 0; i < file_count; i++) {
        free(reports[i].matches);
    }
    free(reports);
}