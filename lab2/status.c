#include "status.h"

const char *status_message(Status status) {
    switch (status) {
        case STATUS_OK: return "успешно";
        case STATUS_ERR_NULL_ARGUMENT: return "передан NULL";
        case STATUS_ERR_FILE_OPEN: return "не удалось открыть файл";
        case STATUS_ERR_FILE_READ: return "ошибка чтения файла";
        case STATUS_ERR_ALLOC_FAILED: return "ошибка выделения памяти";
        case STATUS_ERR_COUNT_NEGATIVE: return "отрицательное количество файлов";
        case STATUS_ERR_SEEK_FAILED: return "ошибка позиционирования в файле";
    }
    return "неизвестная ошибка";
}