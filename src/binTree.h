/* ========= binTree.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define TRUE 1
#define FALSE 0

/* tree node*/
typedef struct tBSTNode
{
    char Key;              /* klíč */
    int BSTNodeCont;       /* užitečný obsah uzlu */
    struct tBSTNode *LPtr; /* levý podstrom */
    struct tBSTNode *RPtr; /* pravý podstrom */
} * tBSTNodePtr;

/* prototypes */

/*Initialize binary tree*/
void BSTInit(tBSTNodePtr *);

/*Find node with given key*/
int BSTSearch(tBSTNodePtr, char, int *);

/*Insert new node with given key. If key is already in tree, update node value*/
void BSTInsert(tBSTNodePtr *, char, int);

/*Remove and replace rightmost node*/
void ReplaceByRightmost(tBSTNodePtr, tBSTNodePtr *);

/*Remove node with key K*/
void BSTDelete(tBSTNodePtr *, char);

/*Dispose of whole tree*/
void BSTDispose(tBSTNodePtr *);
