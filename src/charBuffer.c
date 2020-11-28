/* ========= charBuffer.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Char variable length stack with create, push, get + clear and dispose functions
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "charBuffer.h"
#include "memoryManager.h"

/* Char buffer functions */
charBuffer* charBufferCreate() {
    charBuffer *buffer = malloc(sizeof(charBuffer));

    if (buffer == NULL) {
        throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
        deallocateAll();
        exit(INTERNAL_ERROR);
    }

    buffer->content = malloc(sizeof(char) * INITIAL_CHAR_BUFFER_SIZE);
    buffer->position = 0;
    buffer->capacity = INITIAL_CHAR_BUFFER_SIZE;

    return buffer;
}

void charBufferPush(charBuffer* buffer, char character) {
    if (buffer->position + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        char *newArray = realloc(buffer->content, sizeof(char) * buffer->capacity);

        if (newArray == NULL) {
            free(buffer);
            throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
            deallocateAll();
            exit(INTERNAL_ERROR);
        }
        else {
            buffer->content = newArray;
        }
    }

    buffer->content[(buffer->position)++] = character;
}

char* charBufferGet(charBuffer* buffer) {
    buffer->content[buffer->position] = '\0';
    buffer->position = 0;

    return buffer->content;
}

void charBufferPrint(charBuffer* buffer) {
    buffer->content[buffer->position] = '\0';
    printf("Char buffer: %s\n", buffer->content);
}

void charBufferDispose(charBuffer* buffer) {
    if (buffer != NULL) {
        free(buffer->content);
        free(buffer);
    }
}
