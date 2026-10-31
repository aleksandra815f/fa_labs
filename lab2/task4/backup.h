#ifndef BACKUP_H
#define BACKUP_H

#include "status.h"
#include <stddef.h>

typedef struct {
    char *name;
    unsigned int size;
    unsigned int importance;
} FileDesc;

typedef struct {
    size_t *indices;
    size_t count;
    unsigned long long total_size;
    unsigned long long total_importance;
} BackupResult;

void free_files(FileDesc *array, size_t count);
void free_backup_result(BackupResult *result);
StatusCode read_files_from_stream(void *stream_ptr, FileDesc **array_out, size_t *count_out, unsigned int *capacity_out);
StatusCode select_files_for_backup(const FileDesc *files, size_t n, unsigned int capacity, BackupResult *result);
StatusCode write_result_to_stream(void *stream_ptr, const FileDesc *files, const BackupResult *result);

#endif