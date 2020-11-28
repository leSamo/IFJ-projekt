/* ========= stringBuffer.h ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Descriptions: Char* buffer to register all allocated string to be disposed of at the end of the compiler run
 * ================================= */

#ifndef STRING_BUFFER
#define STRING_BUFFER

#include "constants.h"

#define INITIAL_STRING_BUFFER_SIZE 10

typedef struct StringBuffer {
    int count;
    int capacity;
    char **content;
} StringBuffer;

/* Allocates, initates and returns a string buffer */
StringBuffer* StringBufferCreate();

/* Pushes one token into supplied buffer */
void StringBufferPush(StringBuffer *buffer, char *string);

/* Deallocated token buffer */
void StringBufferDispose(StringBuffer *buffer);

#endif
