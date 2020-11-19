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

Token getToken() {
    if (overlapToken.type != TOK_Empty) {
        Token tokenToReturn = overlapToken;
        overlapToken.type = TOK_Empty;
        return tokenToReturn;
    }

    int currentChar;
    state currentState = AS_Default;

    charBuffer *charBuffer = charBufferCreate();

    char hexEscapeBuffer;

    while (true) {
        currentChar = getchar();

        switch (currentState)
        {
        case AS_Default:
            if (currentChar == '0') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Leading_Zero;
            }
            else if (isDigit(currentChar)) {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Int;
            }
            else if (isAlpha(currentChar) || currentChar == '_') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Word;
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, currentChar);
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
                return newHalfToken(TOK_EOF);
            }
            else if (isWhiteSpace(currentChar)) {
                // if it's whitespace other then newline then noop
                break;
            }
            else {
                printError("Encountered unknown symbol\n");
                throwLexicalError();
            }

            break;

        case AS_Leading_Zero:
            if (currentChar == '0') {
                charBufferPush(charBuffer, currentChar);
            }
            else if (isDigit(currentChar)) {
                printError("Leading zero error\n");
                throwLexicalError();
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Float;
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(charBuffer);
                // TODO: Use 64-bit equivalent to atoi
                return newIntToken(atoi(content));
            }
            break;
        
        case AS_Int:
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, currentChar);
            }
            else if (currentChar == '.') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Float;
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(charBuffer);
                // TODO: Use 64-bit equivalent to atoi
                return newIntToken(atoi(content));
            }
            break;

        case AS_Float:
            // rework float to be double
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, currentChar);
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(charBuffer);
                return newFloatToken(atof(content));
            }
            break;

        case AS_Float_Scientific_Start:
            if (isDigit(currentChar) || currentChar == '+' || currentChar == '-') {
                charBufferPush(charBuffer, currentChar);
                currentState = AS_Float_Scientific;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(charBuffer);
                return newFloatToken(atof(content));
            }
            break;

        case AS_Float_Scientific:
            if (isDigit(currentChar)) {
                charBufferPush(charBuffer, currentChar);
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(charBuffer);
                return newFloatToken(atof(content));
            }
            break;
        
        case AS_Word:
            if (isAlpha(currentChar) || isDigit(currentChar) || currentChar == '_') {
                charBufferPush(charBuffer, currentChar);
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(charBuffer);
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
            else if (currentChar == EOF) {
                return newHalfToken(TOK_EOF);
            }
            break;

        case AS_BlockComm:
            if (currentChar == '*') {
                currentState = AS_BlockComm_End;
            }
            else if (currentChar == EOF) {
                printError("Unexpected EOF inside block comment\n");
                throwLexicalError();
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
                char *content = charBufferGet(charBuffer);
                return newToken(TOK_String_Literal, content);
            }
            else if (currentChar == '\\') {
                currentState = AS_String_Escape;
            }
            else if (currentChar == EOF) {
                printError("Unexpected EOF inside string\n");
                throwLexicalError();
            }
            else {
                charBufferPush(charBuffer, currentChar);
            }
            break;
        
        case AS_String_Escape:
            // TODO: should characters with ascii 31 and less be writable without escape?
            if (currentChar == 'n') {
                charBufferPush(charBuffer, '\n');
                currentState = AS_String;
            }
            else if (currentChar == 't') {
                charBufferPush(charBuffer, '\t');
                currentState = AS_String;
            }
            else if (currentChar == '\\') {
                charBufferPush(charBuffer, '\\');
                currentState = AS_String;
            }
            else if (currentChar == '\"') {
                charBufferPush(charBuffer, '\"');
                currentState = AS_String;
            }
            else if (currentChar == 'x') {
                currentState = AS_String_Escape_Hex_1;
            }
            else {
                printError("Invalid escape sequence\n");
                throwLexicalError();
            }
            break;
        
        case AS_Colon:
            if (currentChar == '=') {
                return newHalfToken(TOK_Define);
            }
            else {
                printError("Invalid : symbol\n");
                throwLexicalError();
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
                printError("Invalid ! symbol\n");
                throwLexicalError();
            }
            break;
        
        case AS_String_Escape_Hex_1:
            if (isHexDigit(currentChar)) {
                hexEscapeBuffer = currentChar;
                currentState = AS_String_Escape_Hex_2;
            }  
            else {
                printError("Invalid hex escape sequence\n");
                throwLexicalError();
            }
            break;

        case AS_String_Escape_Hex_2:
            if (isHexDigit(currentChar)) {
                char charFromHex = hexToChar(hexEscapeBuffer, currentChar);
                charBufferPush(charBuffer, charFromHex);
                currentState = AS_String;
            }  
            else {
                printError("Invalid hex escape sequence\n");
                throwLexicalError();
            }
            break;
        
        default:
            printError("Lexical error: reached invalid state.\n");
            exit(LEXICAL_ERROR);
        }
    }
}

void printError(char* msg) {
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
Token newIntToken(int content) {
    Token newToken;
    newToken.type = TOK_Int_Literal;
    newToken.i = content;

    return newToken;
}

Token newFloatToken(float content) {
    Token newToken;
    newToken.type = TOK_Float_Literal;
    newToken.f = content;

    return newToken;
}

Token newWordToken(char* content) {
    Token newToken;

    for (int i = 0; i < KEYWORDS_ARRAY_SIZE; i++) {
        if (strcmp(content, keywords[i].word) == 0) {
            newToken.type = keywords[i].type;
            return newToken;
        }
    }

    newToken.type = TOK_Identifier;
    newToken.str = malloc(sizeof(char) * (strlen(content) + 1));
    strcpy(newToken.str, content);
    return newToken;
}

Token newToken(tokenType type, char* content) {
    Token newToken;
    newToken.type = type;
    newToken.str = malloc(sizeof(char) * (strlen(content) + 1));
    strcpy(newToken.str, content);

    return newToken;
}

Token newHalfToken(tokenType type) {
    Token newToken;
    newToken.type = type;

    return newToken;
}

void throwLexicalError() {
    printError("Lexical error\n");
    exit(LEXICAL_ERROR);
}

void printToken(Token token) {
    switch (token.type) {
        case TOK_Int_Literal:
            printf("%s: \"%d\"\n", getTokenName(token.type), token.i);
            break;
        case TOK_Float_Literal:
            printf("%s: \"%f\"\n", getTokenName(token.type), token.f);
            break;
        case TOK_Identifier:
        case TOK_String_Literal:
            // TODO: Escape newlines when printing to make output prettier
            printf("%s: \"%s\"\n", getTokenName(token.type), token.str);
            break;
        default:
            printf("%s \n", getTokenName(token.type));
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
        case TOK_EOF:
            return "EOF";
        default:
            return "Unknown       ";
    }
}

charBuffer* charBufferCreate() {
    charBuffer *buffer = malloc(sizeof(charBuffer));

    if (buffer == NULL) {
        printf("Memory allocation error\n");
        exit(INTERNAL_ERROR);
    }

    buffer->content = malloc(sizeof(char) * INITIAL_CHAR_BUFFER_SIZE);
    buffer->position = 0;
    buffer->capacity = INITIAL_CHAR_BUFFER_SIZE;

    return buffer;
}

/* Char buffer functions */
void charBufferPush(charBuffer* buffer, char character) {
    if (buffer->position + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        char *newArray = realloc(buffer->content, sizeof(char) * buffer->capacity);

        if (newArray == NULL) {
            free(buffer);
            printError("Memory allocation error\n");
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
    free(buffer);
}