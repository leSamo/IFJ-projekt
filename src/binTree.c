/* ========= binTree.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#include <stdbool.h>
#include <string.h>
#include "leParser.h"

typedef struct tBSTNode {
    char id[50];
    int depth;
    typeTag Type;
    typeUnion content;
    struct tBSTNode *LPtr;
    struct tBSTNode *RPtr;
} * tBSTNodePtr;

void BSTInit(tBSTNodePtr *RootPtr) { *RootPtr = NULL; }

bool BSTSearch(tBSTNodePtr RootPtr, char *K, typeUnion Content) {
    if (RootPtr == NULL) { // check if tree was initialized
        return false;
    }

    if (strcmp(K, RootPtr->id) == 0) { // key was found
        Content = RootPtr->content;
        return true;
    }

    if (strcmp(K, RootPtr->id) < 0) { // key not foud, move to right/left child
        return BSTSearch(RootPtr->RPtr, K, Content);
    }
    else {
        return BSTSearch(RootPtr->LPtr, K, Content);
    }
}

void BSTInsert(tBSTNodePtr *RootPtr, char *K, typeUnion Content) {
    //printf("%s", K);
    if (*RootPtr == NULL) {        
        tBSTNodePtr new_leaf = malloc(sizeof(struct tBSTNode));
        if (new_leaf == NULL) {           
            return;
        }

        strcpy(new_leaf->id, K);
        // printf("%s", K);
        //new_leaf->BSTNodeCont = Content;
        new_leaf->LPtr = NULL;
        new_leaf->RPtr = NULL;
        *RootPtr = new_leaf;
        return;
    }
    else if (strcmp(K, (*RootPtr)->id) == 0) { // tree already has node with key K
        (*RootPtr)->content = Content;
        return;
    }
    else if (strcmp(K, (*RootPtr)->id) < 0) { // key has smaller/greater value than K, call recursively
        BSTInsert(&(*RootPtr)->RPtr, K, Content);
    }
    else if (strcmp(K, (*RootPtr)->id) > 0) {
        BSTInsert(&(*RootPtr)->LPtr, K, Content);
    }
    return;
}

void BSTDispose(tBSTNodePtr *RootPtr) {
    if (*RootPtr != NULL) {
        BSTDispose(&(*RootPtr)->RPtr);
        BSTDispose(&(*RootPtr)->LPtr);
        free(*RootPtr);
    }
    
    *RootPtr = NULL;
}

void Print_tree2(tBSTNodePtr TempTree, char *sufix, char fromdir)
/* vykresli sktrukturu binarniho stromu */

{
    if (TempTree != NULL) {
        char *suf2 = (char *)malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);
        if (fromdir == 'L') {
            suf2 = strcat(suf2, "  |");
            printf("%s\n", suf2);
        }
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2(TempTree->RPtr, suf2, 'R');
        printf("%s  +-[%s,]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2(TempTree->LPtr, suf2, 'L');
        if (fromdir == 'R')
            printf("%s\n", suf2);
        free(suf2);
    }
}

void Print_tree(tBSTNodePtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        Print_tree2(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}
