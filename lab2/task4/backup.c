#include "backup.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define MAX_DP_CELLS 20000000ull

static size_t string_length(const char *s) {
    size_t len = 0;
    while (s[len] != '\0') ++len;
    return len;
}

static char *string_copy(const char *src, size_t len) {
    char *dst = malloc(len + 1);
    if (dst == NULL) return NULL;
    for (size_t i = 0; i <= len; ++i) dst[i] = src[i];
    return dst;
}

void free_files(FileDesc *array, size_t count) {
    if (array == NULL) return;
    for (size_t i = 0; i < count; ++i) free(array[i].name);
    free(array);
}

void free_backup_result(BackupResult *result) {
    if (result == NULL) return;
    free(result->indices);
    result->indices = NULL;
    result->count = 0;
    result->total_size = 0;
    result->total_importance = 0;
}

StatusCode read_files_from_stream(void *stream_ptr, FileDesc **array_out, size_t *count_out, unsigned int *capacity_out) {
    if (stream_ptr == NULL || array_out == NULL || count_out == NULL || capacity_out == NULL) {
        return STATUS_INVALID_ARGUMENTS;
    }
    *array_out = NULL;
    *count_out = 0;
    *capacity_out = 0;

    FILE *stream = (FILE *)stream_ptr;
    long long n_ll = 0;
    long long cap_ll = 0;
    if (fscanf(stream, "%lld %lld", &n_ll, &cap_ll) != 2) {
        return STATUS_INVALID_DATA_FORMAT;
    }
    if (n_ll <= 0 || (unsigned long long)n_ll > (unsigned long long)(SIZE_MAX / sizeof(FileDesc))) {
        return STATUS_INVALID_DATA_FORMAT;
    }
    if (cap_ll < 0 || (unsigned long long)cap_ll > (unsigned long long)UINT_MAX) {
        return STATUS_INVALID_DATA_FORMAT;
    }

    size_t n = (size_t)n_ll;
    unsigned int cap = (unsigned int)cap_ll;

    FileDesc *files = malloc(n * sizeof(FileDesc));
    if (files == NULL) return STATUS_MEMORY_ERROR;

    char name_buf[4096];
    for (size_t i = 0; i < n; ++i) {
        long long size_ll = 0;
        long long imp_ll = 0;
        if (fscanf(stream, "%4095s %lld %lld", name_buf, &size_ll, &imp_ll) != 3) {
            free_files(files, i);
            return STATUS_INVALID_DATA_FORMAT;
        }
        if (size_ll <= 0 || (unsigned long long)size_ll > (unsigned long long)UINT_MAX) {
            free_files(files, i);
            return STATUS_INVALID_DATA_FORMAT;
        }
        if (imp_ll < 0 || (unsigned long long)imp_ll > (unsigned long long)UINT_MAX) {
            free_files(files, i);
            return STATUS_INVALID_DATA_FORMAT;
        }
        size_t len = string_length(name_buf);
        char *name_copy = string_copy(name_buf, len);
        if (name_copy == NULL) {
            free_files(files, i);
            return STATUS_MEMORY_ERROR;
        }
        files[i].name = name_copy;
        files[i].size = (unsigned int)size_ll;
        files[i].importance = (unsigned int)imp_ll;
    }

    *array_out = files;
    *count_out = n;
    *capacity_out = cap;
    return STATUS_SUCCESS;
}

StatusCode select_files_for_backup(const FileDesc *files, size_t n, unsigned int capacity, BackupResult *result) {
    if (result == NULL) return STATUS_INVALID_ARGUMENTS;
    result->indices = NULL;
    result->count = 0;
    result->total_size = 0;
    result->total_importance = 0;

    if (files == NULL && n > 0) return STATUS_INVALID_ARGUMENTS;
    if (n == 0 || capacity == 0) return STATUS_SUCCESS;

    size_t cols = (size_t)capacity + 1;
    if (cols == 0) return STATUS_TOO_LARGE;
    size_t rows = n + 1;
    if (rows == 0) return STATUS_TOO_LARGE;
    if (rows > SIZE_MAX / cols) return STATUS_TOO_LARGE;
    size_t cells = rows * cols;
    if ((unsigned long long)cells > MAX_DP_CELLS) return STATUS_TOO_LARGE;
    if (cells > SIZE_MAX / sizeof(unsigned long long)) return STATUS_TOO_LARGE;

    unsigned long long *dp = malloc(cells * sizeof(unsigned long long));
    if (dp == NULL) return STATUS_MEMORY_ERROR;

    for (size_t w = 0; w < cols; ++w) dp[w] = 0;

    for (size_t i = 1; i <= n; ++i) {
        size_t s = (size_t)files[i - 1].size;
        unsigned long long v = (unsigned long long)files[i - 1].importance;
        const unsigned long long *prev = dp + (i - 1) * cols;
        unsigned long long *cur = dp + i * cols;
        for (size_t w = 0; w < cols; ++w) {
            if (s > w) {
                cur[w] = prev[w];
            } else {
                unsigned long long take = prev[w - s] + v;
                cur[w] = (take > prev[w]) ? take : prev[w];
            }
        }
    }

    unsigned long long best = dp[n * cols + capacity];

    size_t *indices = malloc(n * sizeof(size_t));
    if (indices == NULL) {
        free(dp);
        return STATUS_MEMORY_ERROR;
    }

    size_t count = 0;
    size_t w = (size_t)capacity;
    unsigned long long total_size = 0;
    for (size_t i = n; i > 0; --i) {
        const unsigned long long *cur = dp + i * cols;
        const unsigned long long *prev = dp + (i - 1) * cols;
        if (cur[w] != prev[w]) {
            indices[count++] = i - 1;
            total_size += files[i - 1].size;
            w -= files[i - 1].size;
        }
    }

    for (size_t a = 0, b = count; a + 1 < b; ++a, --b) {
        size_t t = indices[a];
        indices[a] = indices[b - 1];
        indices[b - 1] = t;
    }

    free(dp);

    if (count == 0) {
        free(indices);
        indices = NULL;
    }

    result->indices = indices;
    result->count = count;
    result->total_size = total_size;
    result->total_importance = best;
    return STATUS_SUCCESS;
}

StatusCode write_result_to_stream(void *stream_ptr, const FileDesc *files, const BackupResult *result) {
    if (stream_ptr == NULL || files == NULL || result == NULL) {
        return STATUS_INVALID_ARGUMENTS;
    }
    FILE *stream = (FILE *)stream_ptr;
    if (fprintf(stream, "TOTAL_SIZE %llu\n", result->total_size) < 0) return STATUS_FILE_WRITE_ERROR;
    if (fprintf(stream, "TOTAL_IMPORTANCE %llu\n", result->total_importance) < 0) return STATUS_FILE_WRITE_ERROR;
    if (fprintf(stream, "FILES %zu\n", result->count) < 0) return STATUS_FILE_WRITE_ERROR;
    for (size_t i = 0; i < result->count; ++i) {
        if (fprintf(stream, "%s\n", files[result->indices[i]].name) < 0) return STATUS_FILE_WRITE_ERROR;
    }
    return STATUS_SUCCESS;
}