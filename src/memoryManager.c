/* ======== memoryManager.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Allocating and disposing of various data structures
 * ================================= */

#include "memoryManager.h"
#include "constants.h"
#include "stringBuffer.c"
#include "charBuffer.h"
#include "AST.c"
#include "tokenBuffer.h"
#include "symtable.h"

StringBuffer *stringBuffer;
charBuffer *scannerBuffer;
ASTNode *ASTRoot;
ASTNode *ASTBuiltIn;
TokenBuffer *tokenBuffer;
ST_Node *SymTableTree;

char* newString(unsigned int length) {
    char* string = malloc(sizeof(char) * (length + 1));
    StringBufferPush(stringBuffer, string);
    return string;
}

void deallocateAll() {
    // free scanner buffer
    charBufferDispose(scannerBuffer);

    // free all strings
    StringBufferDispose(stringBuffer);

    // free semantic AST
    AST_Delete(ASTRoot);

    // free semantic AST with built-in symbols
    AST_Delete(ASTBuiltIn);

    // free token buffer used for recursive descent
    TokenBufferDispose(&tokenBuffer);
}
