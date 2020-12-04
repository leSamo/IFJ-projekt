/* ========= tokenBuffer.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Variable length token buffer with create, push, top, pop, pop front and dispose functions.
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "tokenBuffer.h"

TokenBuffer* TokenBufferCreate() {
    TokenBuffer *buffer = malloc(sizeof(TokenBuffer));
    buffer->tokens = malloc(sizeof(Token) * INITIAL_TOKEN_BUFFER_SIZE);

    if (buffer == NULL || buffer->tokens == NULL) { // at least one allocation failed
        deallocateAll();
        throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_TOKEN_BUFFER_SIZE;

    return buffer;
}

void TokenBufferPush(TokenBuffer *buffer, Token token) {
    if (buffer->count + 1 == buffer->capacity) { // we ran out of space inside buffer
        buffer->capacity *= 2; // double previous capacity
        Token* newArray = realloc(buffer->tokens, sizeof(Token) * buffer->capacity);

        if (newArray == NULL) { // realloc failed
            TokenBufferDispose(&buffer);
            deallocateAll();
            throwError(INTERNAL_ERROR, "Memory allocation error\n", false);
        }
        else {
            buffer->tokens = newArray;
        }

        buffer->tokens[buffer->count++] = token;
    }
    else {
        buffer->tokens[buffer->count++] = token;
    }
}

bool TokenBufferEmpty(TokenBuffer *buffer) {
    return buffer->count == 0;
}

Token TokenBufferTop(TokenBuffer *buffer) {
    if (buffer->count > 0) {
        return buffer->tokens[buffer->count - 1];
    }
    else { // buffer was empty, top operation is invalid
        TokenBufferDispose(&buffer);
        //deallocateAll();
        throwError(SYNTAX_ERROR, "Unbalanced construction error\n", true);
    }
}

Token TokenBufferPop(TokenBuffer *buffer) {
    if (buffer->count > 0) {
        return buffer->tokens[--buffer->count];
    }
    else { // buffer was empty, pop operation is invalid
        TokenBufferDispose(&buffer);
        //deallocateAll();
        throwError(SYNTAX_ERROR, "Unbalanced construction error\n", true);
    }
}

Token TokenBufferPopFront(TokenBuffer **buffer) {
    if ((*buffer)->count > 0) {
        Token returnToken = (*buffer)->tokens[0]; // return first token

        // shift all tokens after first left by one
        for (int i = 0; i < (*buffer)->count - 1; i++) {
            (*buffer)->tokens[i] = (*buffer)->tokens[i + 1];
        }

        (*buffer)->count--;
        return returnToken;
    }
    else { // buffer was empty, pop front operation is invalid
        TokenBufferDispose(buffer);
        //deallocateAll();
        throwError(SYNTAX_ERROR, "Unbalanced construction error\n", true);
    }
}

void TokenBufferPrint(TokenBuffer *buffer) {
    if (buffer->count == 0) {
        printf("Buffer is empty\n");
    }

    for (int i = 0; i < buffer->count; i++) {
        printf("%s\n", getTokenName(buffer->tokens[i].type));
    }
}

void TokenBufferDispose(TokenBuffer **buffer) {
    if (*buffer != NULL) {
        free((*buffer)->tokens);
        free(*buffer);
        *buffer = NULL;
    }
}
