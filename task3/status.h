#ifndef STATUS_H
#define STATUS_H

typedef enum {
    STATUS_OK,
    STATUS_NULL,          
    STATUS_BAD_ARGC,    
    STATUS_BAD_FLAG, 
    STATUS_BAD_NUMBER,  
    STATUS_BAD_EPS, 
    STATUS_ZERO,  
    STATUS_NOT_POSITIVE,
    STATUS_OVERFLOW,
    STATUS_SMALL_BUFFER 
} Status;

#endif