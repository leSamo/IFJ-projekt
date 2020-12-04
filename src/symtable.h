/* ========== symtable.h ===========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#ifndef SYMTABLEH
#define SYMTABLEH

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "intBuffer.h"

/* Of what type is the symbol */
typedef enum {
    SYM_None,
    SYM_Unused,
    SYM_Int,
    SYM_Float,
    SYM_String,
    SYM_Func
} symType;

/* Symtable node AKA symbol */
typedef struct ST_Node {
    char *id;             // used to identify scope for block nodes
    IntBuffer *scopes;    // array of scopes from outermost to innermost
    symType type;
    ASTNode *node;        // which AST node defined this symbol

    struct ST_Node *LPtr; // left child
    struct ST_Node *RPtr; // right child
} ST_Node;

/* Initializes symtable */
void ST_Init(ST_Node **);

/* Fills symtable it with built-in variables and functions */
void ST_SetupBuiltIn(ST_Node **RootPtr);

/* Searches identifier with supplied scopes (or their parent scopes) in the symtable */
ST_Node* ST_Search(ST_Node *RootPtr, ST_Node *CurrentPtr, char *searchedId, IntBuffer scopes);

/* Creates a new symbol and inserts it into the symtable */
void ST_Insert(ST_Node **RootPtr, char *id, symType type, ASTNode *node, IntBuffer scopes);

/* Recursively deallocates all nodes from the symtable */
void ST_Dispose(ST_Node **);

/* [DEBUG] Recursivelly prints symtable, initially level should be 0 */
void ST_PrettyPrint(ST_Node *nodePtr, int level);

#endif
