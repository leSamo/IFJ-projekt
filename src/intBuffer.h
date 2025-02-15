/* ========== intBuffer.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
 * Description: Int variable length array with create, push, compare and dispose functions
 * ================================= */

#ifndef INT_BUFFERH
#define INT_BUFFERH

#define INITIAL_INT_BUFFER_SIZE 10

typedef struct {
    int count;
    int capacity;
    int *content;
} IntBuffer;

/* Allocates, initates and returns a int buffer */
IntBuffer* IntBufferCreate();

/* Pushes one token into supplied buffer */
void IntBufferPush(IntBuffer *buffer, int newItem);

/* Compares two buffers, resulting returned int is similar to strcmp result */
int IntBufferCompare(IntBuffer buffer1, IntBuffer buffer2);

/* Returns string with the contents of the int buffer starting with 2nd element, split with % for use as a prefix */
void IntBufferPrefix(IntBuffer *buffer);

/* [DEBUG] Prints out the contents of the int buffer */
void IntBufferPrint(IntBuffer *buffer);

/* Deallocated token buffer */
void IntBufferDispose(IntBuffer **buffer);

#endif
