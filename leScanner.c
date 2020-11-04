#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "constants.h"
#include "leScanner.h"

int main(int argc, char* argv[]) {
    int currentChar;
    state currentState = AS_Default;

    Token tokens[MAX_TOKEN_ARRAY_SIZE];
    int tokenCount = 0;

    char charBuffer[MAX_CHAR_BUFFER_SIZE];
    int charBufferPos = 0;

    while ((currentChar = getchar()) != EOF) {
        switch (currentState)
        {
        case AS_Default:
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Int;
            }
            else if (isAlpha(currentChar) || currentChar == '_') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Identif;
            }
            else if (currentChar == '/') {
                currentState = AS_Comm_Start;
            }
            else if (currentChar == '\"') {
                currentState = AS_String;
            }

            // if it's whitespace then noop
            break;
        
        case AS_Int:
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float;
            }
            else {
                char *content = charBufferClear(charBuffer, &charBufferPos);
                newIntToken(tokens, &tokenCount, atoi(content));
                currentState = AS_Default;
            }
            break;

        case AS_Float:
            // rework float to be double
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else {
                char *content = charBufferClear(charBuffer, &charBufferPos);
                newFloatToken(tokens, &tokenCount, atof(content));
                currentState = AS_Default;
            }
            break;
        
        case AS_Identif:
            if (isAlpha(currentChar) || isDigit(currentChar) || currentChar == '_') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else {
                char *content = charBufferClear(charBuffer, &charBufferPos);
                newToken(tokens, &tokenCount, TOK_Identif, content);
                currentState = AS_Default;
            }
            break;

        case AS_Comm_Start:
            if (currentChar == '/') {
                currentState = AS_LineComm;
            }
            else if (currentChar == '*') {
                currentState = AS_BlockComm;
            }
            else {
                currentState = AS_Default;
            }
            break;
        
        case AS_LineComm:
            if (currentChar == '\n') {
                currentState = AS_Default;
            }
            break;

        case AS_BlockComm:
            if (currentChar == '*') {
                currentState = AS_BlockComm_End;
            }
            break;
        
        case AS_BlockComm_End:
            if (currentChar == '/') {
                currentState = AS_Default;
            }
            else if (currentChar != '*') {
                currentState = AS_BlockComm;
            }
            break;
        
        case AS_String:
            if (currentChar == '\"') {
                char *content = charBufferClear(charBuffer, &charBufferPos);
                newToken(tokens, &tokenCount, TOK_String, content);
                currentState = AS_Default;
            }
            else if (currentChar == '\\') {
                currentState = AS_String_Escape;
            }
            else {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            break;
        
        case AS_String_Escape:
            // TODO: include all possible escape sequences
            if (currentChar == 'n') {
                charBufferPush(charBuffer, &charBufferPos, '\n');
            }
            else if (currentChar == '\"') {
                charBufferPush(charBuffer, &charBufferPos, '\"');
            }
            currentState = AS_String;
            break;
        
        default:
            throwError("Reached invalid state.\n");
            return 1;
        }
    }

    printAllTokens(tokens, tokenCount);

    return 0;
}

void throwError(char* msg) {
    fprintf(stderr, "%s", msg);
}

/* Current char testing functions */
bool isDigit(char c) {
    return isdigit(c);
}

bool isAlpha(char c) {
    return isalpha(c);
}

bool isWhiteSpace(char c) {
    return isspace(c);
}

/* New token functions */
void newIntToken(Token *tokens, int *tokenCount, int content) {
    Token newToken;
    newToken.type = TOK_Int;
    newToken.i = content;

    tokens[(*tokenCount)++] = newToken;
}

void newFloatToken(Token *tokens, int *tokenCount, float content) {
    Token newToken;
    newToken.type = TOK_Float;
    newToken.f = content;

    tokens[(*tokenCount)++] = newToken;
}

void newToken(Token *tokens, int *tokenCount, tokenType type, char* content) {
    Token newToken;
    newToken.type = type;
    strcpy(newToken.str, content);

    tokens[(*tokenCount)++] = newToken;
}

void newHalfToken(Token *tokens, int *tokenCount, tokenType type) {
    Token newToken;
    newToken.type = type;

    tokens[(*tokenCount)++] = newToken;
}

void printAllTokens(Token *tokens, int tokenCount) {
    printf("\n----- TOKENS -----\n");
    for (int i = 0; i < tokenCount; i++) {
        Token token = tokens[i];
        switch (token.type) {
            case TOK_Int:
                printf("%s: \"%d\"\n", getTokenName(token.type), token.i);
                break;
            case TOK_Float:
                printf("%s: \"%f\"\n", getTokenName(token.type), token.f);
                break;
            case TOK_Identif:
            case TOK_String:
                printf("%s: \"%s\"\n", getTokenName(token.type), token.str);
                break;
        }
    }
    printf("------------------\n\n");
}

char* getTokenName(tokenType type) {
    switch (type) {
        case TOK_Int:
            return "Int       ";
        case TOK_Float:
            return "Float     ";
        case TOK_Identif:
            return "Identifier";
        case TOK_String:
            return "String    ";
        default:
            return "Invalid   ";
    }
}

/* Char buffer functions */
void charBufferPush(char* buffer, int *bufferPos, char character) {
    buffer[(*bufferPos)++] = character;
}

char* charBufferClear(char* buffer, int *bufferPos) {
    buffer[*bufferPos] = '\0';
    char* output = malloc(sizeof(char) * 50);
    strcpy(output, buffer);
    *bufferPos = 0;

    return output;
}

void charBufferPrint(char* buffer, int *bufferPos) {
    buffer[*bufferPos] = '\0';
    printf("Char buffer: %s\n", buffer);
}
