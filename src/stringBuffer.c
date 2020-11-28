/* ========= stringBuffer.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "stringBuffer.h"

StringBuffer* StringBufferCreate() {
    StringBuffer *buffer = malloc(sizeof(StringBuffer));
    buffer->content = malloc(sizeof(char*) * INITIAL_STRING_BUFFER_SIZE);

    if (buffer == NULL) {
        throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
        deallocateAll();
        exit(INTERNAL_ERROR);
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_STRING_BUFFER_SIZE;

    return buffer;
}

void StringBufferPush(StringBuffer *buffer, char *string) {
    if (buffer->count + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        char** newArray = realloc(buffer->content, sizeof(char*) * buffer->capacity);

        if (newArray == NULL) {
            StringBufferDispose(buffer); // most likely unnecessary because deallocateAll does that
            throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
            deallocateAll();
            exit(INTERNAL_ERROR);
        }
        else {
            buffer->content = newArray;
        }

        buffer->content[buffer->count++] = string;
    }
    else {
        buffer->content[buffer->count++] = string;
    }
}

void StringBufferDispose(StringBuffer *buffer) {
    if (buffer != NULL) {
        for (int i = 0; i < buffer->count; i++) {
            free(buffer->content[i]);
        }

        free(buffer->content);
        free(buffer);
    }
}
