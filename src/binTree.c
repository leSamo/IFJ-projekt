/* ========= binTree.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#ifndef BINTREE
#define BINTREE

#include <stdbool.h>
#include <string.h>
#include "leParser.h"

typedef enum {
    SYM_None,
    SYM_Unknown,
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

void ST_Init(ST_Node **RootPtr) {
    *RootPtr = NULL;
}

ST_Node* ST_Search(ST_Node *RootPtr, char *searchedId) {
    if (RootPtr == NULL) {
        return NULL;
    }

    if (strcmp(searchedId, RootPtr->id) == 0) {
        return RootPtr;
    }

    if (strcmp(searchedId, RootPtr->id) < 0) {
        return ST_Search(RootPtr->RPtr, searchedId);
    }
    else {
        return ST_Search(RootPtr->LPtr, searchedId);
    }
}

void ST_Insert(ST_Node **RootPtr, char *id, symType type, ASTNode *node) {
    if (*RootPtr == NULL) {        
        ST_Node *new_leaf = malloc(sizeof(ST_Node));
        new_leaf->id = newString(strlen(id));
        
        // TODO: Check if malloc was successful

        strcpy(new_leaf->id, id);
        new_leaf->type = type;
        new_leaf->node = node;

        new_leaf->LPtr = NULL;
        new_leaf->RPtr = NULL;

        *RootPtr = new_leaf;
    }
    else if (strcmp(id, (*RootPtr)->id) == 0) { // tree already has node with key K
        printError(DEFINITION_TYPE_ERROR, "Variable redefinition error\n");
    }
    else if (strcmp(id, (*RootPtr)->id) < 0) {
        ST_Insert(&(*RootPtr)->RPtr, id, type, node);
    }
    else if (strcmp(id, (*RootPtr)->id) > 0) {
        ST_Insert(&(*RootPtr)->LPtr, id, type, node);
    }
}

void ST_Dispose(ST_Node **RootPtr) {
    if (*RootPtr != NULL) {
        ST_Dispose(&(*RootPtr)->RPtr);
        ST_Dispose(&(*RootPtr)->LPtr);
        free(*RootPtr);
    }
    
    *RootPtr = NULL;
}

void ST_PrettyPrint(ST_Node *nodePtr, int level) {
    if (nodePtr != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        printf("%s\n", nodePtr->id);

        ST_PrettyPrint(nodePtr->LPtr, level + 1);
        ST_PrettyPrint(nodePtr->RPtr, level + 1);
    }
}


#endif
