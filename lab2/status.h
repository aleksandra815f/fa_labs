#ifndef STATUS_H
#define STATUS_H

typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL_ARGUMENT,
    STATUS_ERR_FILE_OPEN,
    STATUS_ERR_FILE_READ,
    STATUS_ERR_ALLOC_FAILED,
    STATUS_ERR_COUNT_NEGATIVE,
    STATUS_ERR_SEEK_FAILED
} Status;

const char *status_message(Status status);

#endif