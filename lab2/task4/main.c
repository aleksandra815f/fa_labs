#include <stdio.h>
#include "status.h"
#include "backup.h"

int main(void) {
    FileDesc *files = NULL;
    size_t count = 0;
    unsigned int capacity = 0;

    StatusCode status = read_files_from_stream(stdin, &files, &count, &capacity);
    if (status != STATUS_SUCCESS) {
        if (status == STATUS_INVALID_DATA_FORMAT) {
            printf("Ошибка: некорректный формат данных или нарушение правил валидации.\n");
        } else if (status == STATUS_MEMORY_ERROR) {
            printf("Ошибка: не удалось выделить динамическую память.\n");
        }
        free_files(files, count);
        return status;
    }

    BackupResult result = {0};
    status = select_files_for_backup(files, count, capacity, &result);
    if (status != STATUS_SUCCESS) {
        if (status == STATUS_MEMORY_ERROR) {
            printf("Ошибка: не удалось выделить динамическую память.\n");
        } else if (status == STATUS_TOO_LARGE) {
            printf("Ошибка: слишком большой размер задачи для метода динамического программирования.\n");
        } else {
            printf("Ошибка при решении задачи.\n");
        }
        free_backup_result(&result);
        free_files(files, count);
        return status;
    }

    status = write_result_to_stream(stdout, files, &result);
    if (status != STATUS_SUCCESS) {
        printf("Ошибка при записи результата.\n");
    }

    free_backup_result(&result);
    free_files(files, count);
    return status;
}