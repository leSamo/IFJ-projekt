/* ========= tokenBuffer.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Variable length token buffer with create, push, top, pop, pop front and dispose functions.
 * ================================= */

#ifndef TOKEN_BUFFER
#define TOKEN_BUFFER

#include "constants.h"

#define INITIAL_TOKEN_BUFFER_SIZE 10

typedef struct TokenBuffer {
    int count;          // how many tokens are inside buffer
    int capacity;       // how many tokens could fit inside buffer before reallocation
    Token *tokens;
} TokenBuffer;

/* Allocates, initates and returns a new token buffer */
TokenBuffer* TokenBufferCreate();

/* Pushes one token into supplied buffer */
void TokenBufferPush(TokenBuffer *buffer, Token token);

/* Returns true is token buffer is empty */
bool TokenBufferEmpty(TokenBuffer *buffer);

/* Returns token on the top of the buffer */
Token TokenBufferTop(TokenBuffer *buffer);

/* Returns n-th token on the top of the buffer */
Token TokenBufferNTop(TokenBuffer *buffer, int n);

/* Returns last inserted token of the buffer and removes it from the buffer */
Token TokenBufferPop(TokenBuffer *buffer);

/* Returns first inserted token of the buffer and removes it from the buffer */
Token TokenBufferPopFront(TokenBuffer **buffer);

/* [DEBUG] Print out all the items from the buffer */
void TokenBufferPrint(TokenBuffer *buffer);

/* Deallocates token buffer */
void TokenBufferDispose(TokenBuffer **buffer);

#endif
