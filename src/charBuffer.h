/* ========= charBuffer.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Char variable length stack with create, push, get + clear and dispose functions
 * ================================= */

#ifndef CHAR_BUFFER
#define CHAR_BUFFER

#define INITIAL_CHAR_BUFFER_SIZE 20

/* inflatable array of characters, has corresponding create, push, get, dispose functions */
typedef struct charBuffer {
    char* content;
    int position;
    int capacity;
} charBuffer;

/* allocates, initializes and returns a new char buffer */
charBuffer* charBufferCreate();

/* pushes character at the end of buffer */
void charBufferPush(charBuffer* buffer, char character);

/* clears buffer and its position and returns its content */
char* charBufferGet(charBuffer* buffer);

/* [DEBUG] Print content of buffer */
void charBufferPrint(charBuffer* buffer);

/* deallocates char buffer */
void charBufferDispose(charBuffer* buffer);

#endif
