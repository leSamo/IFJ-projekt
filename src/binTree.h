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

typedef struct tBSTNode {
    char Key;
    int BSTNodeCont;
    struct tBSTNode *LPtr;
    struct tBSTNode *RPtr;
} *tBSTNodePtr;

/* Initialize binary tree */
void BSTInit(tBSTNodePtr *);

/* Find node with given key */
bool BSTSearch(tBSTNodePtr, char, int *Content);

/* Insert new node with given key. If key is already in tree, update node value */
void BSTInsert(tBSTNodePtr *, char, int);

/* Dispose of whole tree */
void BSTDispose(tBSTNodePtr *);
