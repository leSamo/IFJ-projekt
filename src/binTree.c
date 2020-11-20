/* ========= binTree.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#include "binTree.h"

#include <stdbool.h>

void BSTInit(tBSTNodePtr *RootPtr) { *RootPtr = NULL; }

bool BSTSearch(tBSTNodePtr RootPtr, char K, int *Content) {
    if (RootPtr == NULL) { // check if tree was initialized
        return false;
    }

    if (K == RootPtr->Key) { // key was found
        *Content = RootPtr->BSTNodeCont;
        return true;
    }

    if (K > RootPtr->Key) { // key not foud, move to right/left child
        return BSTSearch(RootPtr->RPtr, K, Content);
    }
    else {
        return BSTSearch(RootPtr->LPtr, K, Content);
    }
}

void BSTInsert(tBSTNodePtr *RootPtr, char K, int Content) {
    if (*RootPtr == NULL) {
        tBSTNodePtr new_leaf = malloc(sizeof(struct tBSTNode));
        if (new_leaf == NULL) {
            return;
        }
        new_leaf->Key = K;
        new_leaf->BSTNodeCont = Content;
        new_leaf->LPtr = NULL;
        new_leaf->RPtr = NULL;
        *RootPtr = new_leaf;
        return;
    }
    else if ((*RootPtr)->Key == K) { // tree already has node with key K
        (*RootPtr)->BSTNodeCont = Content;
        return;
    }
    else if ((*RootPtr)->Key < K) { // key has smaller/greater value than K, call recursively
        BSTInsert(&(*RootPtr)->RPtr, K, Content);
    }
    else if ((*RootPtr)->Key > K) {
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
