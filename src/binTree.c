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
    SYM_FUNC,
    SYM_VAR
} symType;

typedef struct tBSTNode {
    char *id;
    symType type;
    ASTNode *node;

    struct tBSTNode *LPtr;
    struct tBSTNode *RPtr;
} *tBSTNodePtr;

/*
typedef struct tBSTNode {
    char id[50];
    int depth;
    typeTag Type;
    typeUnion content;
    int prefix[5];
    int PrefixCount;
    struct tBSTNode *LPtr;
    struct tBSTNode *RPtr;
} * tBSTNodePtr;
*/

void BSTInit(tBSTNodePtr *RootPtr) {
    *RootPtr = NULL;
}

tBSTNodePtr BSTSearch(tBSTNodePtr RootPtr, char *searchedId) {
    if (RootPtr == NULL) {
        return false;
    }

    if (strcmp(searchedId, RootPtr->id) == 0) {
        return RootPtr;
    }

    if (strcmp(searchedId, RootPtr->id) < 0) {
        return BSTSearch(RootPtr->RPtr, searchedId);
    }
    else {
        return BSTSearch(RootPtr->LPtr, searchedId);
    }
}

void BSTInsert(tBSTNodePtr *RootPtr, char *id, symType type) {
    if (*RootPtr == NULL) {        
        tBSTNodePtr new_leaf = malloc(sizeof(struct tBSTNode));
        new_leaf->id = newString(strlen(id));
        
        // TODO: Check if malloc was successful

        strcpy(new_leaf->id, id);
        new_leaf->type = type;
        new_leaf->LPtr = NULL;
        new_leaf->RPtr = NULL;

        *RootPtr = new_leaf;
    }
    else if (strcmp(id, (*RootPtr)->id) == 0) { // tree already has node with key K
        (*RootPtr)->type = type;
    }
    else if (strcmp(id, (*RootPtr)->id) < 0) {
        BSTInsert(&(*RootPtr)->RPtr, id, type);
    }
    else if (strcmp(id, (*RootPtr)->id) > 0) {
        BSTInsert(&(*RootPtr)->LPtr, id, type);
    }
}

void BSTDispose(tBSTNodePtr *RootPtr) {
    if (*RootPtr != NULL) {
        BSTDispose(&(*RootPtr)->RPtr);
        BSTDispose(&(*RootPtr)->LPtr);
        free(*RootPtr);
    }
    
    *RootPtr = NULL;
}

void BSTPrettyPrint(tBSTNodePtr nodePtr, int level) {
    if (nodePtr != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        printf("%s\n", nodePtr->id);

        BSTPrettyPrint(nodePtr->LPtr, level + 1);
        BSTPrettyPrint(nodePtr->RPtr, level + 1);
    }
}


#endif
