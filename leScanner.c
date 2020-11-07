/* ========== leScanner.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "constants.h"
#include "leScanner.h"

Token *getToken() {
    int currentChar;
    state currentState = AS_Default;

    // TODO: improve buffer to be inflatable array
    char charBuffer[MAX_CHAR_BUFFER_SIZE];
    int charBufferPos = 0;

    char hexEscapeBuffer;

    while (true) {
        currentChar = getchar();

        switch (currentState)
        {
        case AS_Default:
            if (currentChar == '0') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Leading_Zero;
            }
            else if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Int;
            }
            else if (isAlpha(currentChar) || currentChar == '_') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Word;
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float;
            }
            else if (currentChar == '/') {
                currentState = AS_Comm_Start;
            }
            else if (currentChar == '\"') {
                currentState = AS_String;
            }
            else if (currentChar == ',') {
                return newHalfToken(TOK_Comma);
            }
            else if (currentChar == ';') {
                return newHalfToken(TOK_Semicolon);
            }
            else if (currentChar == '(') {
                return newHalfToken(TOK_L_Paren);
            }
            else if (currentChar == ')') {
                return newHalfToken(TOK_R_Paren);
            }
            else if (currentChar == '{') {
                return newHalfToken(TOK_L_Brace);
            }
            else if (currentChar == '}') {
                return newHalfToken(TOK_R_Brace);
            }
            else if (currentChar == '+') {
                return newHalfToken(TOK_Add);
            }
            else if (currentChar == '-') {
                return newHalfToken(TOK_Sub);
            }
            else if (currentChar == '*') {
                return newHalfToken(TOK_Mul);
            }
            else if (currentChar == ':') {
                currentState = AS_Colon;
            }
            else if (currentChar == '=') {
                currentState = AS_Equal;
            }
            else if (currentChar == '<') {
                currentState = AS_Less_Then;
            }
            else if (currentChar == '>') {
                currentState = AS_More_Then;
            }
            else if (currentChar == '!') {
                currentState = AS_Exclamation;
            }
            else if (currentChar == '\n') {
                return newHalfToken(TOK_Newline);
            }
            else if (currentChar == EOF) {
                return NULL;
            }

            // if it's whitespace other then newline then noop
            break;

        case AS_Leading_Zero:
            if (currentChar == '0') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else if (isDigit(currentChar)) {
                throwError("Leading zero error\n");
                return newErrorToken();
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float;
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferPop(charBuffer, &charBufferPos);
                // TODO: Use 64-bit equivalent to atoi
                return newIntToken(atoi(content));
            }
            break;
        
        case AS_Int:
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float;
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferPop(charBuffer, &charBufferPos);
                // TODO: Use 64-bit equivalent to atoi
                return newIntToken(atoi(content));
            }
            break;

        case AS_Float:
            // rework float to be double
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferPop(charBuffer, &charBufferPos);
                return newFloatToken(atof(content));
            }
            break;

        case AS_Float_Scientific_Start:
            if (isDigit(currentChar) || currentChar == '+' || currentChar == '-') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
                currentState = AS_Float_Scientific;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferPop(charBuffer, &charBufferPos);
                return newFloatToken(atof(content));
            }
            break;

        case AS_Float_Scientific:
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferPop(charBuffer, &charBufferPos);
                return newFloatToken(atof(content));
            }
            break;
        
        case AS_Word:
            if (isAlpha(currentChar) || isDigit(currentChar) || currentChar == '_') {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferPop(charBuffer, &charBufferPos);
                return newWordToken(content);
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
                ungetChar(currentChar);
                return newHalfToken(TOK_Div);
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
                char *content = charBufferPop(charBuffer, &charBufferPos);
                return newToken(TOK_String_Literal, content);
            }
            else if (currentChar == '\\') {
                currentState = AS_String_Escape;
            }
            else {
                charBufferPush(charBuffer, &charBufferPos, currentChar);
            }
            break;
        
        case AS_String_Escape:
            // TODO: should characters with ascii 31 and less be writable without escape?
            if (currentChar == 'n') {
                charBufferPush(charBuffer, &charBufferPos, '\n');
                currentState = AS_String;
            }
            else if (currentChar == 't') {
                charBufferPush(charBuffer, &charBufferPos, '\t');
                currentState = AS_String;
            }
            else if (currentChar == '\\') {
                charBufferPush(charBuffer, &charBufferPos, '\\');
                currentState = AS_String;
            }
            else if (currentChar == '\"') {
                charBufferPush(charBuffer, &charBufferPos, '\"');
                currentState = AS_String;
            }
            else if (currentChar == 'x') {
                currentState = AS_String_Escape_Hex_1;
            }
            else {
                throwError("Invalid escape sequence\n");
                return newErrorToken();
            }
            break;
        
        case AS_Colon:
            if (currentChar == '=') {
                return newHalfToken(TOK_Define);
            }
            else {
                throwError("Invalid : symbol\n");
                return newErrorToken();
            }
            break;

        case AS_Equal:
            if (currentChar == '=') {
                return newHalfToken(TOK_Equal);
            }
            else {
                ungetChar(currentChar);
                return newHalfToken(TOK_Assign);
            }

        case AS_Less_Then:
            if (currentChar == '=') {
                return newHalfToken(TOK_Less_Equal_Then);
            }
            else {
                ungetChar(currentChar);
                return newHalfToken(TOK_Less_Then);
            }
            break;

        case AS_More_Then:
            if (currentChar == '=') {
                return newHalfToken(TOK_More_Equal_Then);
            }
            else {
                ungetChar(currentChar);
                return newHalfToken(TOK_More_Then);
            }
            break;
        
        case AS_Exclamation:
            if (currentChar == '=') {
                return newHalfToken(TOK_Not_Equal);
            }
            else {
                throwError("Invalid ! symbol\n");
                return newErrorToken();
            }
            break;
        
        case AS_String_Escape_Hex_1:
            if (isHexDigit(currentChar)) {
                hexEscapeBuffer = currentChar;
                currentState = AS_String_Escape_Hex_2;
            }  
            else {
                throwError("Invalid hex escape sequence\n");
                return newErrorToken();
            }
            break;

        case AS_String_Escape_Hex_2:
            if (isHexDigit(currentChar)) {
                char charFromHex = hexToChar(hexEscapeBuffer, currentChar);
                charBufferPush(charBuffer, &charBufferPos, charFromHex);
                currentState = AS_String;
            }  
            else {
                throwError("Invalid hex escape sequence\n");
                return newErrorToken();
            }
            break;
        
        default:
            throwError("Reached invalid state.\n");
            return NULL;
        }
    }
}

void throwError(char* msg) {
    fprintf(stderr, "%s", msg);
}

void ungetChar(char c) {
    ungetc(c, stdin);
}

/* Current char testing functions */
bool isDigit(char c) {
    return isdigit(c);
}

bool isHexDigit(char c) {
    return isxdigit(c);
}

bool isAlpha(char c) {
    return isalpha(c);
}

bool isWhiteSpace(char c) {
    return isspace(c);
}

char hexToChar(char a, char b) {
    a = (a <= '9') ? a - '0' : (a & 0x7) + 9;
    b = (b <= '9') ? b - '0' : (b & 0x7) + 9;

    return (a << 4) + b;
}

/* New token functions */
Token *newIntToken(int content) {
    Token *newToken = malloc(sizeof(Token));
    newToken->type = TOK_Int_Literal;
    newToken->i = content;

    return newToken;
}

Token *newFloatToken(float content) {
    Token *newToken = malloc(sizeof(Token));
    newToken->type = TOK_Float_Literal;
    newToken->f = content;

    return newToken;
}

Token *newWordToken(char* content) {
    Token *newToken = malloc(sizeof(Token));

    for (int i = 0; i < KEYWORDS_ARRAY_SIZE; i++) {
        if (strcmp(content, keywords[i].word) == 0) {
            newToken->type = keywords[i].type;
            return newToken;
        }
    }

    newToken->type = TOK_Identifier;
    strcpy(newToken->str, content);
    return newToken;
}

Token *newToken(tokenType type, char* content) {
    Token *newToken = malloc(sizeof(Token));
    newToken->type = type;
    strcpy(newToken->str, content);

    return newToken;
}

Token *newHalfToken(tokenType type) {
    Token *newToken = malloc(sizeof(Token));
    newToken->type = type;

    return newToken;
}

Token *newErrorToken() {
    Token *newToken = malloc(sizeof(Token));
    newToken->type = TOK_Error;

    return newToken;
}

void printToken(Token *token) {
    switch (token->type) {
        case TOK_Int_Literal:
            printf("%s: \"%d\"\n", getTokenName(token->type), token->i);
            break;
        case TOK_Float_Literal:
            printf("%s: \"%f\"\n", getTokenName(token->type), token->f);
            break;
        case TOK_Identifier:
        case TOK_String_Literal:
            // TODO: Escape newlines when printing to make output prettier
            printf("%s: \"%s\"\n", getTokenName(token->type), token->str);
            break;
        default:
            printf("%s \n", getTokenName(token->type));
    }
}

char* getTokenName(tokenType type) {
    switch (type) {
        case TOK_Int_Literal:
            return "Int           ";
        case TOK_Float_Literal:
            return "Float         ";
        case TOK_Identifier:
            return "Identifier    ";
        case TOK_String_Literal:
            return "String        ";
        case TOK_Comma:
            return "Comma         ";
        case TOK_Semicolon:
            return "Semicolon     ";
        case TOK_L_Paren:
            return "Left paren    ";
        case TOK_R_Paren:
            return "Right paren   ";
        case TOK_L_Brace:
            return "Left brace    ";
        case TOK_R_Brace:
            return "Right brace   ";
        case TOK_Add:
            return "Addition      ";
        case TOK_Sub:
            return "Subtraction   ";
        case TOK_Mul:
            return "Multiplication";
        case TOK_Div:
            return "Division      ";
        case TOK_Else_Keyword:
            return "Else          ";
        case TOK_Float_Keyword:
            return "Float kw      ";
        case TOK_For_Keyword:
            return "For           ";
        case TOK_Func_Keyword:
            return "Func          ";
        case TOK_If_Keyword:
            return "If            ";
        case TOK_Int_Keyword:
            return "Int kw        ";
        case TOK_Package_Keyword:
            return "Package       ";
        case TOK_Return_Keyword:
            return "Return        ";
        case TOK_String_Keyword:
            return "String kw     ";
        case TOK_Define:
            return "Define        ";
        case TOK_Assign:
            return "Assign        ";
        case TOK_Less_Then:
            return "Less then     ";
        case TOK_Less_Equal_Then:
            return "Less or equal ";
        case TOK_More_Then:
            return "More then     ";
        case TOK_More_Equal_Then:
            return "More or equal ";
        case TOK_Equal:
            return "Equal         ";
        case TOK_Not_Equal:
            return "Not equal     ";
        case TOK_Newline:
            return "--";
        default:
            return "Unknown       ";
    }
}

/* Char buffer functions */
void charBufferPush(char* buffer, int *bufferPos, char character) {
    buffer[(*bufferPos)++] = character;
}

char* charBufferPop(char* buffer, int *bufferPos) {
    buffer[*bufferPos] = '\0';
    *bufferPos = 0;

    return buffer;
}

void charBufferPrint(char* buffer, int *bufferPos) {
    buffer[*bufferPos] = '\0';
    printf("Char buffer: %s\n", buffer);
}
