#ifndef STATUS_H
#define STATUS_H
typedef enum {
    STATUS_OK,
    STATUS_NULL, //NULL вместо адреса
    STATUS_BAD_ARGC,
    STATUS_BAD_FLAG,
    STATUS_BAD_NUMBER,
    STATUS_TOO_BIG,
    STATUS_NEGATIVE,
    STATUS_OUT_OF_RANGE,//для -e
    STATUS_OVERFLOW
}Status;

#endif