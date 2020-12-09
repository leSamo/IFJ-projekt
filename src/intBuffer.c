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
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_INT_BUFFER_SIZE;

    return buffer;
}

void IntBufferPush(IntBuffer *buffer, int newItem) {
    // if we ran out of space in the buffer, allocate new memory with double capacity 
    if (buffer->count + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        int *newArray = realloc(buffer->content, sizeof(int) * buffer->capacity);

        if (newArray == NULL) {
            IntBufferDispose(&buffer);
            throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
        }
        else {
            buffer->content = newArray;
        }
    }
    
    // push int at the end
    buffer->content[buffer->count++] = newItem;
}

int IntBufferCompare(IntBuffer buffer1, IntBuffer buffer2) {
    if (buffer1.count == buffer2.count) {
        // check if element on each position from buffer1 matches element from buffer2
        for (int i = 0; i < buffer1.count; i++) {
            if (buffer1.content[i] != buffer2.content[i]) {
                // tell binary tree traverser whether to search in left or right child
                return (buffer1.content[i] > buffer2.content[i]) ? 1 : -1;
            }
        }

        return 0; // all positions match, they are same
    }
    else {
        // tell binary tree traverser whether to search in left or right child
        return (buffer1.count > buffer2.count) ? 1 : -1;
    }
}

void IntBufferPrefix(IntBuffer *buffer) {
    if (buffer != NULL) {
        // start with 2nd element, first is local scope, which we don't prefix
        for (int i = 1; i < buffer->count; i++) {
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
