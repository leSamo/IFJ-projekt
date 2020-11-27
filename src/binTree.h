/* ========= binTree.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SYM_None,
    SYM_Readonly,
    SYM_Int,
    SYM_Float,
    SYM_String,
    SYM_Func
} symType;

typedef struct ST_Node {
    char *id;
    symType type;
    ASTNode *node;

    struct ST_Node *LPtr;
    struct ST_Node *RPtr;
} ST_Node;

void ST_Init(ST_Node **);

void ST_SetupBuiltIn(ST_Node **RootPtr);

ST_Node* ST_Search(ST_Node *RootPtr, char *searchedId);

void ST_Insert(ST_Node **RootPtr, char *id, symType type, ASTNode *node);

void ST_Dispose(ST_Node **);

void ST_PrettyPrint(ST_Node *nodePtr, int level);
