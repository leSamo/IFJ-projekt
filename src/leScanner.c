/* ========== leScanner.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Emma Krompaščíková (xkromp00)
 * Date: November, December 2020
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "constants.h"
#include "leScanner.h"
#include "charBuffer.c"
#include "memoryManager.c"

// Increments when newline char is read, used in error messages
int currentLine = 1;

// If parser gets one more token than it should have it can give it back with overlap token
Token overlapToken = {TOK_Empty};

Token getToken() {
    // if parser sent back overlap token then return it first and clear it
    if (overlapToken.type != TOK_Empty) {
        Token tokenToReturn = overlapToken;
        overlapToken.type = TOK_Empty;
        return tokenToReturn;
    }

    int currentChar;
    state currentState = AS_Default;

    // used when reading string with escaped ascii char
    char hexEscapeBuffer;

    while (true) {
        currentChar = getchar();

        switch (currentState)
        {
        case AS_Default: // initial state
            if (currentChar == '0') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Leading_Zero;
            }
            else if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Int;
            }
            else if (isAlpha(currentChar) || currentChar == '_') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Word;
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
                currentLine++;
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
                throwError(LEXICAL_ERROR, "Encountered unknown symbol\n", true);
            }

            break;

        case AS_Leading_Zero:
            if (isDigit(currentChar)) {
                throwError(LEXICAL_ERROR, "Leading zero error\n", true);
            }
            else if (currentChar == '.') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Start;
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(scannerBuffer);
                int64_t i = strtoull(content, NULL, 10);

                return newIntToken(i);
            }
            break;
        
        case AS_Int:
            if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
            }
            else if (currentChar == '.') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Start;
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(scannerBuffer);
                int64_t i = strtoull(content, NULL, 10);

                return newIntToken(i);
            }
            break;
        
        case AS_Float_Start:
            if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float;
            }
            else {
                throwError(LEXICAL_ERROR, "Incorrect float literal\n", true);
            }
            break;

        case AS_Float:
            if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
            }
            else if (currentChar == 'e' || currentChar == 'E') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Scientific_Start;
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(scannerBuffer);
                return newFloatToken(atof(content));
            }
            break;

        case AS_Float_Scientific_Start:
            if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Scientific;
            }
            else if (currentChar == '+' || currentChar == '-') {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Scientific_Start_2;
            }
            else {
                throwError(LEXICAL_ERROR, "Incorrect float literal\n", true);
            }
            break;
        
        case AS_Float_Scientific_Start_2:
            if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
                currentState = AS_Float_Scientific;
            }
            else {
                throwError(LEXICAL_ERROR, "Incorrect float literal\n", true);
            }
            break;

        case AS_Float_Scientific:
            if (isDigit(currentChar)) {
                charBufferPush(scannerBuffer, currentChar);
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(scannerBuffer);
                return newFloatToken(atof(content));
            }
            break;
        
        case AS_Word:
            if (isAlpha(currentChar) || isDigit(currentChar) || currentChar == '_') {
                charBufferPush(scannerBuffer, currentChar);
            }
            else {
                ungetChar(currentChar);
                char *content = charBufferGet(scannerBuffer);
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
                currentLine++;
                return newHalfToken(TOK_Newline);
            }
            else if (currentChar == EOF) {
                return newHalfToken(TOK_EOF);
            }
            break;

        case AS_BlockComm:
            if (currentChar == '\n') {
                currentLine++;
            }
            else if (currentChar == '*') {
                currentState = AS_BlockComm_End;
            }
            else if (currentChar == EOF) {
                throwError(LEXICAL_ERROR, "Unexpected EOF inside block comment\n", true);
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
                char *content = charBufferGet(scannerBuffer);
                return newStringToken(content);
            }
            else if (currentChar == '\\') {
                currentState = AS_String_Escape;
            }
            else if (currentChar == EOF) {
                throwError(LEXICAL_ERROR, "Unexpected EOF inside string\n", true);
            }
            else {
                charBufferPush(scannerBuffer, currentChar);
            }
            break;
        
        case AS_String_Escape: // inside string after reading \, initiate escape sequence
            if (currentChar == 'n') {
                charBufferPush(scannerBuffer, '\n');
                currentState = AS_String;
            }
            else if (currentChar == 't') {
                charBufferPush(scannerBuffer, '\t');
                currentState = AS_String;
            }
            else if (currentChar == '\\') {
                charBufferPush(scannerBuffer, '\\');
                currentState = AS_String;
            }
            else if (currentChar == '\"') {
                charBufferPush(scannerBuffer, '\"');
                currentState = AS_String;
            }
            else if (currentChar == 'x') {
                currentState = AS_String_Escape_Hex_1;
            }
            else {
                throwError(LEXICAL_ERROR, "Invalid escape sequence\n", true);
            }
            break;
        
        case AS_Colon:
            if (currentChar == '=') {
                return newHalfToken(TOK_Define);
            }
            else {
                throwError(LEXICAL_ERROR, "Invalid : symbol\n", true);
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
                throwError(LEXICAL_ERROR, "Invalid ! symbol\n", true);
            }
            break;
        
        case AS_String_Escape_Hex_1:
            if (isHexDigit(currentChar)) {
                hexEscapeBuffer = currentChar;
                currentState = AS_String_Escape_Hex_2;
            }  
            else {
                throwError(LEXICAL_ERROR, "Invalid hex escape sequence\n", true);
            }
            break;

        case AS_String_Escape_Hex_2:
            if (isHexDigit(currentChar)) {
                char charFromHex = hexToChar(hexEscapeBuffer, currentChar);
                charBufferPush(scannerBuffer, charFromHex);
                currentState = AS_String;
            }  
            else {
                throwError(LEXICAL_ERROR, "Invalid hex escape sequence\n", true);
            }
            break;
        
        default:
            throwError(LEXICAL_ERROR, "Reached invalid state.\n", true);
        }
    }
}

void throwError(int errorCode, char* msg, bool printLine) {
    fprintf(stderr, "[%d] ", errorCode);

    if (printLine) {
        fprintf(stderr, "Around line %d: ", currentLine);
    }

    fprintf(stderr, "%s", msg);

    deallocateAll();
    exit(errorCode);
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
Token newIntToken(int64_t content) {
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

    // check if identifier matches any keyword
    for (int i = 0; i < KEYWORDS_ARRAY_SIZE; i++) {
        if (strcmp(content, keywords[i].word) == 0) {
            newToken.type = keywords[i].type;
            return newToken;
        }
    }

    newToken.type = TOK_Identifier;
    newToken.str = newString((strlen(content)));
    strcpy(newToken.str, content);

    return newToken;
}

Token newStringToken(char* content) {
    Token newToken;

    newToken.type = TOK_String_Literal;
    newToken.str = newString((strlen(content)));
    strcpy(newToken.str, content);

    return newToken;
}

Token newHalfToken(tokenType type) { // attributeless token
    Token newToken;

    newToken.type = type;

    return newToken;
}

void printToken(Token token) {
    switch (token.type) {
        case TOK_Int_Literal:
            printf("%s: \"%ld\"\n", getTokenName(token.type), token.i);
            break;
        case TOK_Float_Literal:
            printf("%s: \"%f\"\n", getTokenName(token.type), token.f);
            break;
        case TOK_Identifier:
        case TOK_String_Literal:
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
        case TOK_P_$:
            return "$";
        case TOK_P_Greater:
            return ">";
        case TOK_P_Less:
            return "<";
        case TOK_P_Equal:
            return "=";
        case TOK_P_Ex:
            return "Ex";
        default:
            return "Unknown       ";
    }
}
