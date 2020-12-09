/* ========= stringBuffer.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
 * Descriptions: Char* buffer to register all allocated string to be disposed of at the end of the compiler run
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "stringBuffer.h"

StringBuffer* StringBufferCreate() {
    StringBuffer *buffer = malloc(sizeof(StringBuffer));
    buffer->content = malloc(sizeof(char*) * INITIAL_STRING_BUFFER_SIZE);

    if (buffer == NULL) {
        deallocateAll();
        throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_STRING_BUFFER_SIZE;

    return buffer;
}

void StringBufferPush(StringBuffer *buffer, char *string) {
    // if we ran out of space in the buffer, allocate new memory with double capacity 
    if (buffer->count + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        char** newArray = realloc(buffer->content, sizeof(char*) * buffer->capacity);

        if (newArray == NULL) {
            StringBufferDispose(&buffer);
            deallocateAll();
            throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
        }
        else {
            buffer->content = newArray;
        }
    }

    // push char pointer at the end
    buffer->content[buffer->count++] = string;
}

void StringBufferDispose(StringBuffer **buffer) {
    if (*buffer != NULL) {
        // free each pointer inside the buffer
        for (int i = 0; i < (*buffer)->count; i++) {
            free((*buffer)->content[i]);
        }

        // free buffer itself
        free((*buffer)->content);
        free(*buffer);

        *buffer = NULL;
    }
}
