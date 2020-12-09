/* ========== intBuffer.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
 * Description: Int variable length array with create, push, compare and dispose functions
 * ================================= */

#ifndef INT_BUFFER
#define INT_BUFFER

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "intBuffer.h"

IntBuffer* IntBufferCreate() {
    IntBuffer *buffer = malloc(sizeof(IntBuffer));
    buffer->content = malloc(sizeof(int) * INITIAL_INT_BUFFER_SIZE);

    if (buffer == NULL) {
        throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
        deallocateAll();
        exit(INTERNAL_ERROR);
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_INT_BUFFER_SIZE;

    return buffer;
}

void IntBufferPush(IntBuffer *buffer, int newItem) {
    if (buffer->count + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        int *newArray = realloc(buffer->content, sizeof(int) * buffer->capacity);

        if (newArray == NULL) {
            IntBufferDispose(&buffer); // most likely unnecessary because deallocateAll does that
            throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
            deallocateAll();
            exit(INTERNAL_ERROR);
        }
        else {
            buffer->content = newArray;
        }

        buffer->content[buffer->count++] = newItem;
    }
    else {
        buffer->content[buffer->count++] = newItem;
    }
}

int IntBufferCompare(IntBuffer buffer1, IntBuffer buffer2) {
    if (buffer1.count == buffer2.count) {
        for (int i = 0; i < buffer1.count; i++) {
            if (buffer1.content[i] != buffer2.content[i]) {
                return (buffer1.content[i] > buffer2.content[i]) ? 1 : -1;
            }
        }

        return 0; // they are same
    }
    else {
        return (buffer1.count > buffer2.count) ? 1 : -1;
    }
}

void IntBufferPrefix(IntBuffer *buffer) {
    if (buffer != NULL) {
        for (int i = 1; i < buffer->count; i++) { // start with 2nd element, first is local scope
            printf("%%%d", buffer->content[i]);
        }
    }
}

void IntBufferPrint(IntBuffer *buffer) {
    if (buffer->count == 0) {
        printf("buffer is empty");
    }

    for (int i = 0; i < buffer->count; i++) {
        printf("%d ", buffer->content[i]);
    }
}

void IntBufferDispose(IntBuffer **buffer) {
    if (*buffer != NULL) {
        free((*buffer)->content);
        free(*buffer);

        *buffer = NULL;
    }
}

#endif
