/* ======== memoryManager.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
 * Description: Allocating and disposing of various data structures
 * ================================= */

#include "memoryManager.h"
#include "constants.h"
#include "stringBuffer.c"
#include "charBuffer.h"
#include "AST.c"
#include "tokenBuffer.h"
#include "symtable.h"

StringBuffer *stringBuffer; // register of all allocated char* using newString() for to be disposed
charBuffer *scannerBuffer;  // char buffer for saving partial token attribute between scanner states (not between tokens)
TokenBuffer *tokenBuffer;   // token buffer for syntactic analysis of balanced contructions (like multi-assignments)
ASTNode *ASTRoot;           // AST with program structure for semantic analysis
ASTNode *ASTBuiltIn;        // AST with built-in variables and functions for semantic analysis
ST_Node *SymTableTree;      // symbol table for semantic analysis
TokenBuffer *PA_Stack;      // precendece analysis main token stack
TokenBuffer *PA_Input;      // precendece analysis input token stack

char* newString(unsigned int length) {
    char* string = malloc(sizeof(char) * (length + 1));

    // remember pointer to that string for future deallocation
    StringBufferPush(stringBuffer, string);

    return string;
}

void deallocateAll() {
    charBufferDispose(&scannerBuffer);   // free scanner buffer
    StringBufferDispose(&stringBuffer);  // free all strings
    AST_Delete(&ASTRoot);                // free semantic AST
    AST_Delete(&ASTBuiltIn);             // free semantic AST with built-in symbols
    TokenBufferDispose(&tokenBuffer);    // free token buffer used for recursive descent
    TokenBufferDispose(&PA_Stack);       // free precendece analysis main token stack
    TokenBufferDispose(&PA_Input);       // free precendece analysis input token stack
}
