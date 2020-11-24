/* ========= tokenBuffer.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

#ifndef TOKEN_BUFFER
#define TOKEN_BUFFER

#include "constants.h"

#define INITIAL_TOKEN_BUFFER_SIZE 10

typedef struct TokenBuffer {
    int count;
    int capacity;
    Token *tokens;
} TokenBuffer;

/* Allocates, initates and returns a token buffer */
TokenBuffer* TokenBufferCreate();

/* Pushes one token into supplied buffer */
void TokenBufferPush(TokenBuffer *buffer, Token token);

/* Returns true is token buffer is empty */
bool TokenBufferEmpty(TokenBuffer *buffer);

/* Returns token on the top of the buffer */
Token TokenBufferTop(TokenBuffer *buffer);

/* Returns token on the top of the buffer and removes it from the buffer */
Token TokenBufferPop(TokenBuffer *buffer);

/* Debug function to print out all the items from the buffer */
void TokenBufferPrint(TokenBuffer *buffer);

/* Deallocated token buffer */
void TokenBufferDispose(TokenBuffer *buffer);

#endif
